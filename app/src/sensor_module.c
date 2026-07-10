#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(sensor_module, LOG_LEVEL_INF);

#define SENSOR_THREAD_STACK_SIZE 1024
#define SENSOR_THREAD_PRIORITY 5
#define SENSOR_PERIOD_MS 1000

K_MSGQ_DEFINE(sensor_msgq, sizeof(sensor_sample_t), SENSOR_QUEUE_MAX_ITEMS, 4);

static struct k_thread sensor_thread_data;
K_THREAD_STACK_DEFINE(sensor_thread_stack, SENSOR_THREAD_STACK_SIZE);

static int32_t next_sensor_value(bool fault_mode_enabled)
{
    static int32_t normal_value = 20;

    if (fault_mode_enabled) {
        return 999;
    }

    int32_t current = normal_value;
    normal_value++;
    if (normal_value > 30) {
        normal_value = 20;
    }

    return current;
}

static void sensor_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t seq = 0;

    while (1) {
        system_state_t state = app_state_get_copy();

        sensor_sample_t sample = {
            .value = next_sensor_value(state.fault_mode_enabled),
            .seq = seq,
            .timestamp_ms = k_uptime_get(),
            .injected_fault = state.fault_mode_enabled,
        };

        int ret = k_msgq_put(&sensor_msgq, &sample, K_NO_WAIT);
        if (ret == 0) {
            app_state_set_latest_value(sample.value);
            app_state_inc_samples_produced();
            app_state_update_sensor_seen(sample.timestamp_ms);
            LOG_INF("Produced sample seq=%u value=%d fault=%d",
                    sample.seq, sample.value, sample.injected_fault);
        } else {
            app_state_inc_queue_drops();
            LOG_WRN("Sensor queue full, dropped seq=%u", sample.seq);
        }

        seq++;
        k_msleep(SENSOR_PERIOD_MS);
    }
}

int sensor_module_init(void)
{
    k_tid_t tid = k_thread_create(&sensor_thread_data,
                                  sensor_thread_stack,
                                  K_THREAD_STACK_SIZEOF(sensor_thread_stack),
                                  sensor_thread_entry,
                                  NULL, NULL, NULL,
                                  SENSOR_THREAD_PRIORITY,
                                  0,
                                  K_NO_WAIT);

    if (tid == NULL) {
        return -1;
    }

    k_thread_name_set(tid, "sensor_thread");
    return 0;
}

struct k_msgq *sensor_module_get_msgq(void)
{
    return &sensor_msgq;
}
