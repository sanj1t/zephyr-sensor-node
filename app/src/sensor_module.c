#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(sensor_module, LOG_LEVEL_INF);

#define SENSOR_PERIOD_MS 1000
#define SENSOR_QUEUE_MAX_ITEMS 8

K_MSGQ_DEFINE(sensor_msgq, sizeof(sensor_sample_t), SENSOR_QUEUE_MAX_ITEMS, 4);

static struct k_timer sensor_timer;
static struct k_work_delayable sensor_work;

static uint32_t sample_seq;
static int32_t normal_value = 20;

static int32_t generate_value(fault_type_t type)
{
    switch (type) {
    case FAULT_RANGE:
        return 999;

    case FAULT_STUCK:
        return normal_value;

    case FAULT_NOISY:
        return normal_value + (sample_seq % 5);

    case FAULT_NONE:
    default:
        break;
    }

    int32_t value = normal_value;
    normal_value++;
    if (normal_value > 30) {
        normal_value = 20;
    }

    return value;
}

static void sensor_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    system_state_t state = app_state_get_copy();

    int32_t value = generate_value(state.fault_type);

    sensor_sample_t sample = {
        .value = value,
        .seq = sample_seq,
        .timestamp_ms = k_uptime_get(),
        .injected_fault = state.fault_mode_enabled,
    };

    int ret = k_msgq_put(&sensor_msgq, &sample, K_NO_WAIT);
    if (ret == 0) {
        app_state_set_latest_value(sample.value);
        app_state_inc_samples_produced();
        app_state_update_sensor_seen(sample.timestamp_ms);

        if (state.fault_mode_enabled) {
            uint32_t now = k_uptime_get_32();
            uint32_t duration = now - state.fault_start_time_ms;
            LOG_WRN("Fault sample seq=%u type=%d value=%d duration=%u ms",
                    sample.seq, state.fault_type, sample.value, duration);
        } else {
            LOG_INF("Produced sample seq=%u value=%d fault=0",
                    sample.seq, sample.value);
        }
    } else {
        app_state_inc_queue_drops();
        LOG_WRN("Sensor queue full, dropped seq=%u", sample.seq);
    }

    sample_seq++;
    k_work_schedule(&sensor_work, K_MSEC(SENSOR_PERIOD_MS));
}

static void sensor_timer_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    k_work_schedule(&sensor_work, K_NO_WAIT);
}

int sensor_module_init(void)
{
    sample_seq = 0;
    normal_value = 20;

    k_work_init_delayable(&sensor_work, sensor_work_handler);

    k_timer_init(&sensor_timer, sensor_timer_handler, NULL);
    k_timer_start(&sensor_timer, K_MSEC(SENSOR_PERIOD_MS), K_MSEC(SENSOR_PERIOD_MS));

    return 0;
}

struct k_msgq *sensor_module_get_msgq(void)
{
    return &sensor_msgq;
}
