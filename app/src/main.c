#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"
#include "sensor_module.h"
#include "command_module.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
    int ret = app_state_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize app state: %d", ret);
        return ret;
    }

    ret = sensor_module_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize sensor module: %d", ret);
        return ret;
    }

    ret = command_module_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize command module: %d", ret);
        return ret;
    }

    LOG_INF("zephyr-sensor-node booting on native_sim");
    LOG_INF("Day 3: command thread active; try 'help' or 'status'");

    struct k_msgq *sensor_q = sensor_module_get_msgq();

    while (1) {
        sensor_sample_t sample;
        ret = k_msgq_get(sensor_q, &sample, K_FOREVER);
        if (ret == 0) {
            app_state_inc_samples_processed();
            app_state_update_processing_seen(k_uptime_get());

            system_state_t state = app_state_get_copy();

            LOG_INF("Consumed sample seq=%u value=%d produced=%u processed=%u drops=%u fault_mode=%d",
                    sample.seq,
                    sample.value,
                    state.samples_produced,
                    state.samples_processed,
                    state.queue_drops,
                    state.fault_mode_enabled);
        }
    }

    return 0;
}
