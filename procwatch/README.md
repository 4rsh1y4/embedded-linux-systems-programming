# Procwatch - Local System Monitor

This project is a local system monitor for Linux written in C++. It features an interactive terminal mode and a background daemon mode integrated with `systemd`.

## Directory Structure
* `src/`: Contains the main source code (`main.cpp`) and the sample `config` file.
* `include/`: Contains the header file (`procwatch.hpp`).
* `procwatch.service`: The systemd unit file for background execution.
* `Makefile` & `CMakeLists.txt`: Build automation files.

## Build Instructions

You can build the project using either `Make` or `CMake`.

### Using Make

```bash
make
```

To clean the build files:
```bash
make clean
```

### Using CMake

```bash
mkdir build && cd build
cmake ..
make
```

## How to Run

### 1. Interactive Mode
Run the executable with the `interactive` argument. This mode displays system stats and the Top-N CPU-consuming processes in the terminal, updating every 2 seconds.

```bash
./procwatch interactive
```

### 2. Background Mode (systemd Service)
To run the program as a background service, use the provided `install` target in the Makefile to move the files to their standard Linux directories.

**Installation:**
```bash
sudo make install
```

**Start the service:**
```bash
sudo systemctl start procwatch
sudo systemctl enable procwatch
```

**Monitoring the log:**
```bash
journalctl -u procwatch -f
```

**To test crash recovery:**
1. Edit `/etc/procwatch.conf` and set `CRASH_TEST=1`.
2. Restart the service using `sudo systemctl restart procwatch`.
3. Monitor logs to see the service crash and automatically recover after 5 seconds.

**To Uninstall:**
```bash
sudo make uninstall
```