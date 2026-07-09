#include <string.h>
#include <zephyr/kernel.h>
#include "app_state.h"

static system_state_t g_state;
static struct k_mutex g_state_mutex;

static void app_state_lock(void)
{
    k_mutex_lock(&g_state_mutex, K_FOREVER);
}

static void app_state_unlock(void)
{
    k_mutex_unlock(&g_state_mutex);
}

int app_state_init(void)
{
    int ret = k_mutex_init(&g_state_mutex);
    if (ret != 0) {
        return ret;
    }

    memset(&g_state, 0, sizeof(g_state));
    g_state.active_fault_type = FAULT_NONE;

    int64_t now_ms = k_uptime_get();
    g_state.sensor_last_seen_ms = now_ms;
    g_state.processing_last_seen_ms = now_ms;
    g_state.command_last_seen_ms = now_ms;
    g_state.supervisor_last_seen_ms = now_ms;

    return 0;
}

system_state_t app_state_get_copy(void)
{
    system_state_t copy;

    app_state_lock();
    copy = g_state;
    app_state_unlock();

    return copy;
}

void app_state_set_latest_value(int32_t value)
{
    app_state_lock();
    g_state.latest_value = value;
    app_state_unlock();
}

void app_state_inc_samples_produced(void)
{
    app_state_lock();
    g_state.samples_produced++;
    app_state_unlock();
}

void app_state_inc_samples_processed(void)
{
    app_state_lock();
    g_state.samples_processed++;
    app_state_unlock();
}

void app_state_inc_queue_drops(void)
{
    app_state_lock();
    g_state.queue_drops++;
    app_state_unlock();
}

void app_state_inc_heartbeat(void)
{
    app_state_lock();
    g_state.heartbeat_count++;
    app_state_unlock();
}

void app_state_set_fault_mode(bool enabled, fault_type_t type)
{
    app_state_lock();
    g_state.fault_mode_enabled = enabled;
    g_state.active_fault_type = enabled ? type : FAULT_NONE;
    app_state_unlock();
}

void app_state_clear_faults(void)
{
    app_state_lock();
    g_state.fault_mode_enabled = false;
    g_state.fault_active = false;
    g_state.active_fault_type = FAULT_NONE;
    app_state_unlock();
}

void app_state_set_fault_active(bool active)
{
    app_state_lock();
    g_state.fault_active = active;
    app_state_unlock();
}

void app_state_update_sensor_seen(int64_t now_ms)
{
    app_state_lock();
    g_state.sensor_last_seen_ms = now_ms;
    app_state_unlock();
}

void app_state_update_processing_seen(int64_t now_ms)
{
    app_state_lock();
    g_state.processing_last_seen_ms = now_ms;
    app_state_unlock();
}

void app_state_update_command_seen(int64_t now_ms)
{
    app_state_lock();
    g_state.command_last_seen_ms = now_ms;
    app_state_unlock();
}

void app_state_update_supervisor_seen(int64_t now_ms)
{
    app_state_lock();
    g_state.supervisor_last_seen_ms = now_ms;
    app_state_unlock();
}
