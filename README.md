# zephyr-sensor-node

Zephyr RTOS project built on `native_sim` to demonstrate embedded application structure, shared state management, inter-thread communication and fault-oriented design patterns.

## Current scope
- Custom Zephyr application scaffold
- Shared state module protected with a mutex
- Logging-based runtime on `native_sim`

## Build
```bash
source ~/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=~/zephyrproject/zephyr
cd app
west build -b native_sim . --pristine
west build -t run
```
