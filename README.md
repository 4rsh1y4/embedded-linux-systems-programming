# Embedded Linux Systems Programming

A collection of **Embedded Linux coursework projects** written in C++ for a Real-Time Embedded Systems course.

The repository contains two practical exercises focused on Linux systems programming:

- a local client/server application using **Unix domain sockets**
- a Linux system monitor, **procwatch**, built directly on the `/proc` filesystem and integrated with `systemd`

The original coursework specification is available in [`docs/assignment-specification.pdf`](docs/assignment-specification.pdf).

## Projects

### 1. Unix Domain Socket IPC

[`unix-domain-socket/`](unix-domain-socket/)

A small request/response client-server application demonstrating local inter-process communication using Unix domain sockets.

The server listens on a local `AF_UNIX` stream socket and remains available for successive client connections. The client connects from the command line and sends one of two text-based requests:

```text
time
echo <message>
```

The server either returns the current system time or echoes the supplied message.

**Concepts covered**

- Unix domain sockets
- POSIX socket API
- Local inter-process communication
- Client/server programming
- Simple application-level protocol design
- Command-line programs
- Make

---

### 2. Procwatch — Linux System Monitor

[`procwatch/`](procwatch/)

`procwatch` is a Linux system-monitoring application that obtains system and process information directly from the `/proc` pseudo-filesystem.

It supports both interactive execution and background operation as a `systemd` service.

#### Interactive mode

Periodically displays system information including:

- kernel version
- CPU core count
- overall CPU utilization
- memory usage
- system load averages
- top-N processes by CPU activity
- process resident memory usage (RSS)

CPU utilization is calculated from the difference between consecutive `/proc/stat` samples rather than from a single instantaneous read.

Process activity is similarly determined by comparing CPU accounting information from successive `/proc/<pid>/stat` samples.

#### Background mode

In background mode, `procwatch`:

- reads monitoring settings from a configuration file
- periodically collects top-N process information
- writes output to the system journal
- handles graceful termination through `SIGTERM`
- supports intentional failure for testing service restart behavior
- runs under `systemd` with restart-on-failure behavior

The application can be built using either **Make** or **CMake**.

**Concepts covered**

- Linux `/proc` pseudo-filesystem
- CPU utilization sampling
- Process enumeration and accounting
- Memory monitoring
- POSIX signals
- Configuration-file parsing
- syslog / journald
- `systemd`
- Make
- CMake
- C++ systems programming

## Repository Structure

```text
.
├── docs/
│   └── assignment-specification.pdf
│
├── unix-domain-socket/
│   ├── client.cpp
│   ├── server.cpp
│   └── Makefile
│
└── procwatch/
    ├── include/
    ├── src/
    ├── Makefile
    ├── CMakeLists.txt
    ├── procwatch.service
    └── ...
```

## Academic Context

These programs were originally developed as practical coursework for a **Real-Time Embedded Systems** course.

The repository has been retained and documented as an academic portfolio project to demonstrate Linux systems-programming concepts and the implementation work completed for the course.

The original submitted version is preserved in the Git tag:

```text
coursework-submission
```

## Technologies

`C++` · `Linux` · `/proc` · `POSIX` · `Unix Domain Sockets` · `systemd` · `Make` · `CMake`