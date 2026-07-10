cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_sensor_node)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
    src/main.c
    src/app_state.c
    src/sensor_module.c
    src/command_module.c
)
