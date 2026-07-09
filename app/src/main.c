#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
    int ret = app_state_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize app state: %d", ret);
        return ret;
    }

    system_state_t state = app_state_get_copy();

    LOG_INF("zephyr-sensor-node booting on native_sim");
    LOG_INF("Initial state: latest=%d produced=%u processed=%u fault_mode=%d fault_active=%d",
            state.latest_value,
            state.samples_produced,
            state.samples_processed,
            state.fault_mode_enabled,
            state.fault_active);

    while (1) {
        k_sleep(K_SECONDS(5));
        LOG_INF("Day 1 skeleton alive");
    }

    return 0;
}
