#include "sensor_logic.h"

int32_t sensor_logic_generate(fault_type_t type, uint32_t seq, int32_t *normal_value)
{
    switch (type) {
    case FAULT_RANGE:
        return SENSOR_FAULT_RANGE_VALUE;

    case FAULT_STUCK:
        return *normal_value;

    case FAULT_NOISY:
        return *normal_value + (int32_t)(seq % 5);

    case FAULT_NONE:
    default:
        break;
    }

    int32_t value = *normal_value;

    (*normal_value)++;
    if (*normal_value > SENSOR_NORMAL_MAX) {
        *normal_value = SENSOR_NORMAL_MIN;
    }

    return value;
}
