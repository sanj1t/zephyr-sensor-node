#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    FAULT_NONE,
    FAULT_RANGE,
    FAULT_STUCK,
    FAULT_NOISY,
} fault_type_t;

typedef struct {
    int32_t value;
    uint32_t seq;
    int64_t timestamp_ms;
    bool injected_fault;
} sensor_sample_t;

typedef struct {
    int32_t latest_value;
    uint32_t samples_produced;
    uint32_t samples_processed;
    uint32_t queue_drops;
    uint32_t heartbeat_count;

    bool fault_mode_enabled;
    bool fault_active;
    fault_type_t fault_type;
    uint32_t fault_start_time_ms;
    uint32_t fault_duration_ms;

    int64_t last_sensor_seen_ms;
    int64_t last_processing_seen_ms;
    int64_t last_command_seen_ms;
} system_state_t;

#endif
