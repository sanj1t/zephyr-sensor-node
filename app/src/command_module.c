#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>
#include "command_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(command_module, LOG_LEVEL_INF);

#define CMD_THREAD_STACK_SIZE 1536
#define CMD_THREAD_PRIORITY 6
#define CMD_BUF_SIZE 64

static struct k_thread cmd_thread_data;
K_THREAD_STACK_DEFINE(cmd_thread_stack, CMD_THREAD_STACK_SIZE);

static const struct device *console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static void print_help(void)
{
    LOG_INF("Commands: help | status | fault on | fault off");
}

static void handle_command(const char *cmd)
{
    if (strcmp(cmd, "help") == 0) {
        print_help();
    } else if (strcmp(cmd, "status") == 0) {
        system_state_t state = app_state_get_copy();
        LOG_INF("STATUS latest=%d produced=%u processed=%u drops=%u hb=%u fault_mode=%d fault_active=%d",
                state.latest_value,
                state.samples_produced,
                state.samples_processed,
                state.queue_drops,
                state.heartbeat_count,
                state.fault_mode_enabled,
                state.fault_active);
    } else if (strcmp(cmd, "fault on") == 0) {
        app_state_set_fault_mode(true, FAULT_RANGE);
        app_state_set_fault_active(true);
        LOG_WRN("Fault mode enabled");
    } else if (strcmp(cmd, "fault off") == 0) {
        app_state_clear_faults();
        LOG_INF("Fault mode disabled");
    } else if (strlen(cmd) > 0) {
        LOG_WRN("Unknown command: %s", cmd);
        print_help();
    }
}

static void command_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!device_is_ready(console_dev)) {
        LOG_ERR("Console UART device not ready");
        return;
    }

    char buf[CMD_BUF_SIZE];
    size_t idx = 0;

    LOG_INF("Command thread ready; type 'help'");

    while (1) {
        unsigned char c;
        int ret = uart_poll_in(console_dev, &c);

        if (ret == 0) {
            app_state_update_command_seen(k_uptime_get());

            if (c == '\r' || c == '\n') {
                buf[idx] = '\0';
                handle_command(buf);
                idx = 0;
                memset(buf, 0, sizeof(buf));
            } else if (idx < (CMD_BUF_SIZE - 1)) {
                buf[idx++] = (char)c;
            } else {
                LOG_WRN("Command too long, clearing buffer");
                idx = 0;
                memset(buf, 0, sizeof(buf));
            }
        } else {
            k_msleep(20);
        }
    }
}

int command_module_init(void)
{
    k_tid_t tid = k_thread_create(&cmd_thread_data,
                                  cmd_thread_stack,
                                  K_THREAD_STACK_SIZEOF(cmd_thread_stack),
                                  command_thread_entry,
                                  NULL, NULL, NULL,
                                  CMD_THREAD_PRIORITY,
                                  0,
                                  K_NO_WAIT);

    if (tid == NULL) {
        return -1;
    }

    k_thread_name_set(tid, "command_thread");
    return 0;
}
