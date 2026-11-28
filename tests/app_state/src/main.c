#include <zephyr/ztest.h>
#include "app_state.h"

static void reset_state(void *fixture)
{
    ARG_UNUSED(fixture);
    zassert_equal(app_state_init(), 0, "app_state_init failed");
}

ZTEST_SUITE(app_state, NULL, NULL, reset_state, NULL, NULL);

ZTEST(app_state, test_init_zeroes_state)
{
    system_state_t s = app_state_get_copy();

    zassert_equal(s.latest_value, 0, NULL);
    zassert_equal(s.samples_produced, 0, NULL);
    zassert_equal(s.samples_processed, 0, NULL);
    zassert_equal(s.queue_drops, 0, NULL);
    zassert_equal(s.heartbeat_count, 0, NULL);
    zassert_false(s.fault_mode_enabled, NULL);
    zassert_false(s.fault_active, NULL);
    zassert_equal(s.fault_type, FAULT_NONE, NULL);
}

ZTEST(app_state, test_counters_increment)
{
    app_state_inc_samples_produced();
    app_state_inc_samples_produced();
    app_state_inc_samples_processed();
    app_state_inc_queue_drops();
    app_state_inc_heartbeat();
    app_state_inc_heartbeat();
    app_state_inc_heartbeat();

    system_state_t s = app_state_get_copy();

    zassert_equal(s.samples_produced, 2, NULL);
    zassert_equal(s.samples_processed, 1, NULL);
    zassert_equal(s.queue_drops, 1, NULL);
    zassert_equal(s.heartbeat_count, 3, NULL);
}

ZTEST(app_state, test_set_latest_value)
{
    app_state_set_latest_value(27);
    zassert_equal(app_state_get_copy().latest_value, 27, NULL);

    app_state_set_latest_value(-5);
    zassert_equal(app_state_get_copy().latest_value, -5, NULL);
}

ZTEST(app_state, test_fault_mode_enable_records_type)
{
    app_state_set_fault_mode(true, FAULT_STUCK);

    system_state_t s = app_state_get_copy();

    zassert_true(s.fault_mode_enabled, NULL);
    zassert_equal(s.fault_type, FAULT_STUCK, NULL);
    zassert_true(s.fault_start_time_ms <= k_uptime_get_32(), NULL);
}

ZTEST(app_state, test_fault_mode_disable_resets_type)
{
    app_state_set_fault_mode(true, FAULT_NOISY);
    app_state_set_fault_mode(false, FAULT_NONE);

    system_state_t s = app_state_get_copy();

    zassert_false(s.fault_mode_enabled, NULL);
    zassert_equal(s.fault_type, FAULT_NONE, NULL);
    zassert_equal(s.fault_start_time_ms, 0, NULL);
}

ZTEST(app_state, test_clear_faults_resets_everything_fault_related)
{
    app_state_set_fault_mode(true, FAULT_RANGE);
    app_state_set_fault_active(true);

    app_state_clear_faults();

    system_state_t s = app_state_get_copy();

    zassert_false(s.fault_mode_enabled, NULL);
    zassert_false(s.fault_active, NULL);
    zassert_equal(s.fault_type, FAULT_NONE, NULL);
    zassert_equal(s.fault_start_time_ms, 0, NULL);
}

ZTEST(app_state, test_clear_faults_keeps_counters)
{
    app_state_inc_samples_produced();
    app_state_set_fault_mode(true, FAULT_RANGE);

    app_state_clear_faults();

    zassert_equal(app_state_get_copy().samples_produced, 1,
                  "clearing faults must not reset counters");
}

ZTEST(app_state, test_timestamps_are_stored)
{
    app_state_update_sensor_seen(1000);
    app_state_update_processing_seen(2000);
    app_state_update_command_seen(3000);

    system_state_t s = app_state_get_copy();

    zassert_equal(s.last_sensor_seen_ms, 1000, NULL);
    zassert_equal(s.last_processing_seen_ms, 2000, NULL);
    zassert_equal(s.last_command_seen_ms, 3000, NULL);
}

ZTEST(app_state, test_copy_is_a_snapshot)
{
    app_state_set_latest_value(10);
    system_state_t before = app_state_get_copy();

    app_state_set_latest_value(99);

    zassert_equal(before.latest_value, 10,
                  "an earlier copy must not change when state changes");
    zassert_equal(app_state_get_copy().latest_value, 99, NULL);
}
