# zephyr-sensor-node ![CI](https://github.com/sanj1t/zephyr-sensor-node/actions/workflows/ci.yaml/badge.svg)
A Zephyr RTOS application for `native_sim` that demonstrates a modular embedded software architecture: periodic sensor sampling, inter-thread communication, mutex-protected shared state, runtime fault injection, and a Zephyr shell command interface. It includes a ztest unit-test suite run with Twister.

## Overview

The application implements a small sensor pipeline:

- A periodic timer triggers sensor sampling.
- A delayable work item produces a sensor sample.
- The sample is placed into a message queue.
- A semaphore signals the consumer that new data is available.
- The main thread consumes the sample and updates shared state.
- A Zephyr shell provides runtime commands for status, statistics, and fault injection.

The project is built to show common Zephyr RTOS patterns in a small, readable codebase.

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

Sample generation is kept in a separate pure module, `sensor_logic.c`. It has no kernel dependencies, so it can be unit tested in isolation. `sensor_module.c` handles the timer, work item, and queue, and calls into `sensor_logic.c` to produce each value.

## Features

- Timer-driven periodic sampling every 1000 ms.
- Producer/consumer pipeline using `k_msgq` and a semaphore.
- Shared application state protected by a mutex.
- Three injectable fault types: `range`, `stuck`, and `noisy`.
- Fault duration tracking and warning-level logging while a fault is active.
- Queue drop counting.
- Zephyr shell command interface under a single `app` root command.
- Unit tests for the sensor value logic using ztest and Twister.
- Runs on `native_sim` with no hardware required.
- GitHub Actions CI that builds the app and runs the Twister suite on every push.

## Fault types

| Fault | Shell command | Behavior |
|---|---|---|
| None | `app fault_off` | Values cycle through 20–30 |
| Range | `app fault_range` | Out-of-range value (`999`) |
| Stuck | `app fault_stuck` | Value stops changing |
| Noisy | `app fault_noisy` | Value varies with the sample sequence number |

## Shell commands

| Command | Description |
|---|---|
| `app status` | Current health and state: latest value, fault mode and type, last activity timestamps, queue health |
| `app stats` | Cumulative counters: samples produced and processed, queue drops, heartbeat count, fault duration |
| `app fault_range` | Enable the range fault |
| `app fault_stuck` | Enable the stuck fault |
| `app fault_noisy` | Enable the noisy fault |
| `app fault_off` | Clear all faults and resume normal operation |

Example:

```text
sensor-node:~$ app status
STATUS
  latest_value: 25
  fault_mode: disabled
  fault_type: none
  fault_active: no
  last_sensor_seen_ms: 12345
  last_processing_seen_ms: 12345
  queue_health: healthy

sensor-node:~$ app stats
STATS
  samples_produced: 42
  samples_processed: 42
  queue_drops: 0
  heartbeat_count: 42
  fault_duration_ms: 0
```

## Build and run

Prerequisites: a working Zephyr workspace (this project was developed against a Zephyr development build, `v4.4.0-7914-g7608f5eb4565`) and the Zephyr SDK or a host toolchain capable of building `native_sim`.

```bash
source ~/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=~/zephyrproject/zephyr
cd ~/zephyr-sensor-node/app
west build -b native_sim . --pristine
west build -t run
```

The run prints a pseudo-terminal path for the shell UART, for example `/dev/pts/1`. In a second terminal, attach to it:

```bash
screen /dev/pts/1
```

Press Enter to get the shell prompt, then run `help` or any `app` command. To exit `screen`, press `Ctrl+A`, then `k`, then `y`.

## Testing

Unit tests live in `tests/` and cover the sensor value generation logic: the normal 20–30 cycle and wrap-around, plus the range, stuck, and noisy fault behaviors.

```bash
source ~/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=~/zephyrproject/zephyr
cd ~/zephyr-sensor-node
west twister -T tests -p native_sim -v
```

Twister writes its results to `twister-out/`, which is git-ignored.

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
│   │   ├── sensor_logic.h
│   │   └── sensor_module.h
│   └── src/
│       ├── app_state.c
│       ├── command_module.c
│       ├── main.c
│       ├── sensor_logic.c
│       └── sensor_module.c
├── tests/
│   └── sensor_logic/
│       ├── CMakeLists.txt
│       ├── prj.conf
│       ├── testcase.yaml
│       └── src/
│           └── main.c
├── west.yml
├── .github/
│   └── workflows/
│       └── ci.yml
├── .gitignore
├── LICENSE
└── README.md
```

## What this demonstrates

- Zephyr application structure with separated modules and shared type definitions.
- Kernel objects in a working pipeline: timers, delayable work, message queues, semaphores, and mutexes.
- Thread-safe shared state accessed through copy-out snapshots.
- Producer/consumer design with explicit overflow handling and drop counting.
- Runtime diagnostics through the Zephyr shell, with a clear split between current status and cumulative statistics.
- Fault injection for exercising error-handling paths without hardware.
- Separation of pure logic from kernel code so it can be unit tested with ztest and Twister.
- Simulation-first embedded development with `native_sim`.

## Future work

- Add unit tests for the shared state module.
- Add a drift fault type.
- Add task watchdog supervision of the producer and consumer paths, reported through `app status`.
- Add a real hardware target.

## License

MIT License. See `LICENSE` for details.
