#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include "app_types.h"

int app_state_init(void);
system_state_t app_state_get_copy(void);

void app_state_set_latest_value(int32_t value);
void app_state_inc_samples_produced(void);
void app_state_inc_samples_processed(void);
void app_state_inc_queue_drops(void);
void app_state_inc_heartbeat(void);

void app_state_set_fault_mode(bool enabled, fault_type_t type);
void app_state_clear_faults(void);
void app_state_set_fault_active(bool active);

void app_state_update_sensor_seen(int64_t now_ms);
void app_state_update_processing_seen(int64_t now_ms);
void app_state_update_command_seen(int64_t now_ms);
void app_state_update_supervisor_seen(int64_t now_ms);

#endif
