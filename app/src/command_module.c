#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include "command_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(command_module, LOG_LEVEL_INF);

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    system_state_t state = app_state_get_copy();

    shell_print(sh, "STATUS latest=%d produced=%u processed=%u drops=%u hb=%u fault_mode=%d fault_active=%d",
                state.latest_value,
                state.samples_produced,
                state.samples_processed,
                state.queue_drops,
                state.heartbeat_count,
                state.fault_mode_enabled,
                state.fault_active);

    return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    system_state_t state = app_state_get_copy();

    shell_print(sh, "STATS latest=%d produced=%u processed=%u drops=%u hb=%u fault_mode=%d fault_active=%d",
                state.latest_value,
                state.samples_produced,
                state.samples_processed,
                state.queue_drops,
                state.heartbeat_count,
                state.fault_mode_enabled,
                state.fault_active);

    return 0;
}

static int cmd_fault_on(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    app_state_set_fault_mode(true, FAULT_RANGE);
    app_state_set_fault_active(true);

    shell_print(sh, "Fault mode enabled");

    return 0;
}

static int cmd_fault_off(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    app_state_clear_faults();

    shell_print(sh, "Fault mode disabled");

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(app_cmds,
    SHELL_CMD(status, NULL, "Print system status", cmd_status),
    SHELL_CMD(stats, NULL, "Print system stats", cmd_stats),
    SHELL_CMD(fault_on, NULL, "Enable fault mode", cmd_fault_on),
    SHELL_CMD(fault_off, NULL, "Disable fault mode", cmd_fault_off),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(app, &app_cmds, "Application commands", NULL);

int command_module_init(void)
{
    return 0;
}
