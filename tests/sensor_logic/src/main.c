#include <zephyr/ztest.h>
#include "sensor_logic.h"

ZTEST(sensor_logic, test_normal_cycles_20_to_30)
{
    int32_t normal = SENSOR_NORMAL_MIN;

    for (int i = 0; i <= (SENSOR_NORMAL_MAX - SENSOR_NORMAL_MIN); i++) {
        int32_t value = sensor_logic_generate(FAULT_NONE, i, &normal);

        zassert_equal(value, SENSOR_NORMAL_MIN + i,
                      "expected %d, got %d", SENSOR_NORMAL_MIN + i, value);
    }

    zassert_equal(normal, SENSOR_NORMAL_MIN,
                  "normal value should wrap back to %d", SENSOR_NORMAL_MIN);
}

ZTEST(sensor_logic, test_range_fault_returns_out_of_range)
{
    int32_t normal = SENSOR_NORMAL_MIN;

    zassert_equal(sensor_logic_generate(FAULT_RANGE, 0, &normal),
                  SENSOR_FAULT_RANGE_VALUE, "range fault must return %d",
                  SENSOR_FAULT_RANGE_VALUE);

    zassert_equal(normal, SENSOR_NORMAL_MIN,
                  "range fault must not advance the normal value");
}

ZTEST(sensor_logic, test_stuck_fault_holds_value)
{
    int32_t normal = 25;

    zassert_equal(sensor_logic_generate(FAULT_STUCK, 0, &normal), 25, NULL);
    zassert_equal(sensor_logic_generate(FAULT_STUCK, 1, &normal), 25, NULL);
    zassert_equal(normal, 25, "stuck fault must not advance the normal value");
}

ZTEST(sensor_logic, test_noisy_fault_varies_with_sequence)
{
    int32_t normal = 25;

    zassert_equal(sensor_logic_generate(FAULT_NOISY, 0, &normal), 25, NULL);
    zassert_equal(sensor_logic_generate(FAULT_NOISY, 3, &normal), 28, NULL);
    zassert_equal(normal, 25, "noisy fault must not advance the normal value");
}

ZTEST_SUITE(sensor_logic, NULL, NULL, NULL, NULL, NULL);
