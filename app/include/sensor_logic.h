#ifndef SENSOR_LOGIC_H
#define SENSOR_LOGIC_H

#include <stdint.h>
#include "app_types.h"

#define SENSOR_NORMAL_MIN 20
#define SENSOR_NORMAL_MAX 30
#define SENSOR_FAULT_RANGE_VALUE 999

int32_t sensor_logic_generate(fault_type_t type, uint32_t seq, int32_t *normal_value);

#endif
