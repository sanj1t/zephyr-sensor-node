#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"

LOG_MODULE_REGISTER(app_state, LOG_LEVEL_INF);

static struct k_mutex state_lock;
static system_state_t app_state;

int app_state_init(void)
{
    k_mutex_init(&state_lock);

    k_mutex_lock(&state_lock, K_FOREVER);
    memset(&app_state, 0, sizeof(app_state));
    app_state.fault_type = FAULT_NONE;
    k_mutex_unlock(&state_lock);

    return 0;
}

system_state_t app_state_get_copy(void)
{
    system_state_t copy;

    k_mutex_lock(&state_lock, K_FOREVER);
    copy = app_state;
    k_mutex_unlock(&state_lock);

    return copy;
}

void app_state_set_latest_value(int32_t value)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.latest_value = value;
    k_mutex_unlock(&state_lock);
}

void app_state_inc_samples_produced(void)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.samples_produced++;
    k_mutex_unlock(&state_lock);
}

void app_state_inc_samples_processed(void)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.samples_processed++;
    k_mutex_unlock(&state_lock);
}

void app_state_inc_queue_drops(void)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.queue_drops++;
    k_mutex_unlock(&state_lock);
}

void app_state_inc_heartbeat(void)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.heartbeat_count++;
    k_mutex_unlock(&state_lock);
}

void app_state_set_fault_mode(bool enabled, fault_type_t type)
{
    k_mutex_lock(&state_lock, K_FOREVER);

    if (enabled) {
        app_state.fault_mode_enabled = true;
        app_state.fault_type = type;
        app_state.fault_start_time_ms = k_uptime_get_32();
    } else {
        app_state.fault_mode_enabled = false;
        app_state.fault_type = FAULT_NONE;
        app_state.fault_start_time_ms = 0;
    }

    k_mutex_unlock(&state_lock);
}

void app_state_set_fault_active(bool active)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.fault_active = active;
    k_mutex_unlock(&state_lock);
}

void app_state_clear_faults(void)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.fault_mode_enabled = false;
    app_state.fault_active = false;
    app_state.fault_type = FAULT_NONE;
    app_state.fault_start_time_ms = 0;
    k_mutex_unlock(&state_lock);
}

void app_state_update_sensor_seen(int64_t now_ms)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.last_sensor_seen_ms = now_ms;
    k_mutex_unlock(&state_lock);
}

void app_state_update_processing_seen(int64_t now_ms)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.last_processing_seen_ms = now_ms;
    k_mutex_unlock(&state_lock);
}

void app_state_update_command_seen(int64_t now_ms)
{
    k_mutex_lock(&state_lock, K_FOREVER);
    app_state.last_command_seen_ms = now_ms;
    k_mutex_unlock(&state_lock);
}
