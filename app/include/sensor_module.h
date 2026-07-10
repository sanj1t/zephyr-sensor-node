#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <zephyr/kernel.h>
#include "app_types.h"

#define SENSOR_QUEUE_MAX_ITEMS 8

int sensor_module_init(void);
struct k_msgq *sensor_module_get_msgq(void);

#endif
