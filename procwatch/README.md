# Procwatch — Linux System Monitor

`procwatch` is a C++ Linux system-monitoring application that reads system and process information directly from the Linux `/proc` pseudo-filesystem.

It was developed as part of a **Real-Time Embedded Systems** course to explore Linux system interfaces, process accounting, resource monitoring, signals, background services, and build systems.

The application supports two operating modes:

- **Interactive mode** — periodically displays system and process information in the terminal.
- **Background mode** — periodically collects process information while running as a `systemd` service and writes its output to the system journal.

## Features

### System Monitoring

In interactive mode, `procwatch` reports:

- Linux kernel information
- Number of CPU cores
- Overall CPU utilization
- Memory usage
- System load averages

The information is obtained directly from files exposed through `/proc` rather than by invoking utilities such as `top` or `free`.

### CPU Utilization

CPU counters exposed by `/proc/stat` are cumulative values, so a single read cannot provide CPU utilization over a period of time.

`procwatch` takes consecutive samples and calculates utilization from the change in CPU counters between those samples.

Conceptually:

```text
total_delta = total_current - total_previous
idle_delta  = idle_current  - idle_previous

cpu_usage = (total_delta - idle_delta) / total_delta
```

This allows the program to estimate overall CPU utilization during each monitoring interval.

### Process Monitoring

Running processes are discovered by enumerating numeric directories under:

```text
/proc/<pid>/
```

For each process, `procwatch` obtains information including:

- PID
- process name
- CPU activity
- Resident Set Size (RSS)

Process CPU activity is determined by sampling the process CPU counters from:

```text
/proc/<pid>/stat
```

and comparing the combined `utime + stime` values between consecutive samples.

The resulting delta is used to rank processes according to CPU activity.

RSS information is obtained from process information exposed through `/proc`.

By default, the monitor displays the **top 5** processes, although the number of processes can be configured.

> The per-process value represents CPU activity between samples. It is not presented as a normalized CPU percentage.

### Handling Short-Lived Processes

Processes may terminate while `/proc` is being inspected.

For example, a process can exist when `/proc` is enumerated but disappear before its statistics are read.

`procwatch` is designed to tolerate these cases rather than treating a disappearing process as a fatal error.

## Operating Modes

### Interactive Mode

Interactive mode runs directly in the terminal:

```bash
./procwatch interactive
```

The display is periodically refreshed with updated system statistics and the most active processes.

This mode is useful for observing system behavior and comparing results with tools such as `top` or `htop`.

### Background Mode

Background mode is intended for continuous execution:

```bash
./procwatch background <config-file>
```

The configuration file controls monitoring parameters such as:

- sampling interval
- number of processes to report

In this mode, process information is periodically written to the system log rather than displayed as an interactive terminal table.

The program also handles `SIGTERM` so that a service stop can result in a controlled shutdown.

A deliberate failure mechanism is included to allow `systemd` restart-on-failure behavior to be tested.

## systemd Integration

The repository includes a `procwatch.service` unit for running the monitor as a Linux system service.

The intended service behavior is:

```text
systemd
   |
   +-- starts procwatch in background mode
   |
   +-- procwatch reads its configuration
   |
   +-- monitoring data is written to the journal
   |
   +-- SIGTERM -> graceful shutdown
   |
   +-- abnormal exit -> restart after a delay
```

The service is configured to restart only after failure rather than after a normal shutdown.

Service installation and runtime behavior should be tested on a Linux system before deployment.

## Building

`procwatch` supports both **Make** and **CMake**.

### Make

```bash
make
```

Then run:

```bash
./procwatch interactive
```

Clean generated files with:

```bash
make clean
```

### CMake

Configure the project:

```bash
cmake -S . -B build
```

Build it:

```bash
cmake --build build
```

The resulting executable can then be run in interactive or background mode.

## Project Structure

```text
procwatch/
├── include/
│   └── procwatch.hpp
├── src/
│   └── main.cpp
├── CMakeLists.txt
├── Makefile
├── procwatch.service
└── README.md
```

## Linux Interfaces Used

### `/proc/stat`

Used to obtain cumulative CPU accounting information for calculating overall CPU utilization between samples.

### `/proc/meminfo`

Used to obtain system memory information.

### `/proc/loadavg`

Used to obtain Linux load averages.

### `/proc/<pid>/stat`

Used to obtain process CPU accounting information, including `utime` and `stime`.

### `/proc/<pid>/status`

Used to obtain process information such as resident memory usage.

## Technologies and Concepts

- C++
- Linux
- Embedded Linux
- `/proc` pseudo-filesystem
- Process enumeration
- CPU accounting
- Memory monitoring
- POSIX signals
- syslog / journald
- `systemd`
- Make
- CMake

## Academic Context

This project was developed as a practical assignment for a **Real-Time Embedded Systems** course.

The objective was to gain hands-on experience with Embedded Linux facilities rather than to build a replacement for production monitoring tools such as `top`, `htop`, or dedicated observability systems.

The assignment covered:

- reading Linux system information directly from `/proc`
- calculating CPU activity from consecutive samples
- enumerating and monitoring processes
- handling processes that disappear during sampling
- implementing interactive and background execution modes
- configuration-file parsing
- signal handling
- system logging
- running an application as a `systemd` service
- building the same application using Make and CMake

The original coursework version is preserved in the repository under the `coursework-submission` Git tag.

## Limitations

This implementation was created for educational purposes and intentionally has a limited scope.

In particular:

- process CPU activity is used primarily for relative ranking rather than as a fully normalized CPU percentage
- monitoring is based on periodic polling of `/proc`
- the application targets Linux systems that expose the required procfs interfaces
- it is not intended as a production monitoring or observability platform