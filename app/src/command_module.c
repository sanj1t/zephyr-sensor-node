#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "command_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(command_module, LOG_LEVEL_INF);

static const char *fault_type_name(fault_type_t type)
{
    switch (type) {
    case FAULT_RANGE:
        return "range";
    case FAULT_STUCK:
        return "stuck";
    case FAULT_NOISY:
        return "noisy";
    case FAULT_NONE:
    default:
        return "none";
    }
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    system_state_t state = app_state_get_copy();
    bool queue_healthy = (state.queue_drops == 0);

    shell_print(sh, "STATUS");
    shell_print(sh, "  latest_value: %d", state.latest_value);
    shell_print(sh, "  fault_mode: %s",
                state.fault_mode_enabled ? "enabled" : "disabled");
    shell_print(sh, "  fault_type: %s", fault_type_name(state.fault_type));
    shell_print(sh, "  fault_active: %s",
                state.fault_active ? "yes" : "no");
    shell_print(sh, "  last_sensor_seen_ms: %lld",
                (long long)state.last_sensor_seen_ms);
    shell_print(sh, "  last_processing_seen_ms: %lld",
                (long long)state.last_processing_seen_ms);
    shell_print(sh, "  queue_health: %s",
                queue_healthy ? "healthy" : "drops detected");

    return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    system_state_t state = app_state_get_copy();
    uint32_t fault_duration_ms = 0;

    if (state.fault_mode_enabled) {
        fault_duration_ms = k_uptime_get_32() - state.fault_start_time_ms;
    }

    shell_print(sh, "STATS");
    shell_print(sh, "  samples_produced: %u", state.samples_produced);
    shell_print(sh, "  samples_processed: %u", state.samples_processed);
    shell_print(sh, "  queue_drops: %u", state.queue_drops);
    shell_print(sh, "  heartbeat_count: %u", state.heartbeat_count);
    shell_print(sh, "  fault_duration_ms: %u", fault_duration_ms);

    return 0;
}

static int cmd_fault_range(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    app_state_set_fault_mode(true, FAULT_RANGE);
    app_state_set_fault_active(true);

    shell_print(sh, "Fault mode enabled: range");

    return 0;
}

static int cmd_fault_stuck(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    app_state_set_fault_mode(true, FAULT_STUCK);
    app_state_set_fault_active(true);

    shell_print(sh, "Fault mode enabled: stuck");

    return 0;
}

static int cmd_fault_noisy(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    app_state_set_fault_mode(true, FAULT_NOISY);
    app_state_set_fault_active(true);

    shell_print(sh, "Fault mode enabled: noisy");

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
    SHELL_CMD(status, NULL, "Show current system health and state", cmd_status),
    SHELL_CMD(stats, NULL, "Show cumulative runtime counters", cmd_stats),
    SHELL_CMD(fault_range, NULL, "Enable range fault", cmd_fault_range),
    SHELL_CMD(fault_stuck, NULL, "Enable stuck fault", cmd_fault_stuck),
    SHELL_CMD(fault_noisy, NULL, "Enable noisy fault", cmd_fault_noisy),
    SHELL_CMD(fault_off, NULL, "Disable fault mode", cmd_fault_off),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(app, &app_cmds, "Application commands", NULL);

int command_module_init(void)
{
    return 0;
}
