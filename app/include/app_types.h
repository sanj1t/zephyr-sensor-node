#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FAULT_NONE = 0,
    FAULT_RANGE,
    FAULT_STUCK,
    FAULT_TIMEOUT
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
    fault_type_t active_fault_type;
    int64_t sensor_last_seen_ms;
    int64_t processing_last_seen_ms;
    int64_t command_last_seen_ms;
    int64_t supervisor_last_seen_ms;
} system_state_t;

#endif
