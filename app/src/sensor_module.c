#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "sensor_module.h"
#include "app_state.h"

LOG_MODULE_REGISTER(sensor_module, LOG_LEVEL_INF);

#define SENSOR_THREAD_PRIORITY 5
#define SENSOR_PERIOD_MS 1000
#define SENSOR_QUEUE_MAX_ITEMS 8

K_MSGQ_DEFINE(sensor_msgq, sizeof(sensor_sample_t), SENSOR_QUEUE_MAX_ITEMS, 4);

/* Timer + delayable work for periodic sampling */
static struct k_timer sensor_timer;
static struct k_work_delayable sensor_work;

static uint32_t sample_seq;

static void sensor_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    static int32_t normal_value = 20;

    system_state_t state = app_state_get_copy();

    int32_t value = state.fault_mode_enabled ? 999 : normal_value;

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
        LOG_INF("Produced sample seq=%u value=%d fault=%d",
                sample.seq, sample.value, sample.injected_fault);
    } else {
        app_state_inc_queue_drops();
        LOG_WRN("Sensor queue full, dropped seq=%u", sample.seq);
    }

    /* Advance state for next sample */
    sample_seq++;
    if (!state.fault_mode_enabled) {
        normal_value++;
        if (normal_value > 30) {
            normal_value = 20;
        }
    }

    /* Reschedule work for next period */
    k_work_schedule(&sensor_work, K_MSEC(SENSOR_PERIOD_MS));
}

static void sensor_timer_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);

    /* Just submit work to run in thread context */
    k_work_schedule(&sensor_work, K_NO_WAIT);
}

int sensor_module_init(void)
{
    sample_seq = 0;

    /* Initialize delayable work */
    k_work_init_delayable(&sensor_work, sensor_work_handler);

    /* Initialize and start periodic timer */
    k_timer_init(&sensor_timer, sensor_timer_handler, NULL);
    k_timer_start(&sensor_timer, K_MSEC(SENSOR_PERIOD_MS), K_MSEC(SENSOR_PERIOD_MS));

    return 0;
}

struct k_msgq *sensor_module_get_msgq(void)
{
    return &sensor_msgq;
}
