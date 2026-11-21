# zephyr-sensor-node

A Zephyr RTOS application built for `native_sim` that demonstrates a modular embedded software architecture with periodic sensor sampling, inter-thread communication, shared state management, and a shell-based command interface.

## Overview

The application implements a small sensor pipeline:

- A periodic timer triggers sensor sampling.
- A delayable work item produces a sensor sample.
- The sample is placed into a message queue.
- The main thread consumes the sample and updates shared state.
- A semaphore signals the consumer when new data is available.
- A Zephyr shell provides runtime commands for status, statistics, and fault control.

This project is designed to demonstrate common Zephyr RTOS patterns in a clean, interview-ready codebase.

## Architecture

```text
+-------------------+        +-------------------+
|   Timer + Work    |        |   Main Thread     |
|  (Sensor Module)  |        |  (Consumer)       |
+---------+---------+        +---------+---------+
          |                            ^
          |  sensor_sample_t           |
          +------------> k_msgq -------+
          |                            |
          +------------> semaphore ----+
                                       |
                            +----------+----------+
                            |     Shared State    |
                            |   (app_state.c)     |
                            +----------+----------+
                                       ^
                                       |
                            +----------+----------+
                            |     Zephyr Shell    |
                            |   (command_module)  |
                            +---------------------+
```

## Project structure

```text
zephyr-sensor-node/
├── app/
│   ├── CMakeLists.txt
│   ├── prj.conf
│   ├── include/
│   │   ├── app_state.h
│   │   ├── app_types.h
│   │   ├── command_module.h
│   │   └── sensor_module.h
│   └── src/
│       ├── app_state.c
│       ├── command_module.c
│       ├── main.c
│       └── sensor_module.c
└── README.md
```

## Build and run

From the Zephyr workspace:

```bash
source ~/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=~/zephyrproject/zephyr
cd ~/zephyr-sensor-node/app
west build -b native_sim . --pristine
west build -t run
```

## Shell commands

Once the application is running, use the Zephyr shell:

```text
app status
app stats
app fault_on
app fault_off
```

Example:

```text
sensor-node:~$ app status
STATUS latest=25 produced=12 processed=12 drops=0 hb=12 fault_mode=0 fault_active=0
```

## Current features

- Timer-driven periodic sampling.
- Producer/consumer pattern using `k_msgq`.
- Semaphore-based signaling between producer and consumer.
- Shared state protected with a mutex.
- Runtime fault-mode control.
- Zephyr shell commands for status and statistics.
- Logging via Zephyr logging subsystem.
- Runs on `native_sim` without physical hardware.

## What this demonstrates

- Zephyr RTOS application structure.
- Kernel objects: timers, workqueues, message queues, semaphores, mutexes.
- Thread-safe shared state.
- Producer/consumer design.
- Runtime command interface using Zephyr shell.
- Logging and diagnostics.
- Simulation-first embedded development with `native_sim`.

## Future work

- Add more realistic fault types (stuck, noisy, drift).
- Add unit tests using `ztest`.
- Add CI workflow for automated builds.
- Add a real hardware target.
- Add structured logging and metrics.

## License

MIT License. See `LICENSE` for details.
