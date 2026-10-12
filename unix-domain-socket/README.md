# Unix Domain Socket Client/Server

A small C++ client/server application demonstrating **local inter-process communication (IPC)** on Linux using Unix domain sockets.

This program was developed as part of a **Real-Time Embedded Systems** course to practice the POSIX socket API and simple request/response communication between local processes.

## Overview

The project contains two programs:

- `server` — creates a Unix domain socket, waits for client connections, processes requests, and returns responses
- `client` — connects to the server, sends a command supplied through the command line, prints the response, and exits

Unlike TCP/IP sockets, Unix domain sockets are intended for communication between processes on the same host and use a filesystem socket path instead of an IP address and port.

## Supported Commands

The application implements a small text-based protocol with two commands.

### Current Time

```text
time
```

The server returns the current system date and time.

Example:

```bash
./client time
```

### Echo

```text
echo <message>
```

The server returns the text supplied by the client.

Example:

```bash
./client echo Hello from the client
```

Example response:

```text
Hello from the client
```

## Communication Flow

The basic request/response sequence is:

```text
Client                            Server
  |                                 |
  |                           create socket
  |                           bind socket path
  |                           listen
  |                                 |
  |-------- connect --------------->|
  |                                 |
  |-------- request --------------->|
  |                                 |
  |<------- response ---------------|
  |                                 |
  |-------- close ----------------->|
  |                                 |
  |                           wait for next client
```

The server remains active after serving a client and waits for subsequent connections.

## Unix Domain Socket API

The implementation uses a stream-oriented Unix domain socket:

```cpp
socket(AF_UNIX, SOCK_STREAM, 0)
```

The server follows the usual socket lifecycle:

```text
socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv()
   ↓
send()
   ↓
close()
```

The client follows:

```text
socket()
   ↓
connect()
   ↓
send()
   ↓
recv()
   ↓
close()
```

## Building

The project includes a `Makefile` that builds both the client and server.

```bash
make
```

This produces the two executables:

```text
server
client
```

To remove generated build files:

```bash
make clean
```

## Running

This application targets Linux or another Unix-like environment supporting Unix domain sockets.

Start the server in one terminal:

```bash
./server
```

Then use another terminal to send requests.

For example:

```bash
./client time
```

or:

```bash
./client echo Hello world
```

The server remains running and can handle additional client requests sequentially.

## Project Structure

```text
unix-domain-socket/
├── client.cpp
├── server.cpp
├── Makefile
└── README.md
```

## Concepts Demonstrated

- C++
- Linux systems programming
- Inter-process communication
- Unix domain sockets
- POSIX socket API
- Client/server architecture
- Request/response protocols
- Command-line interfaces
- Make

## Academic Context

This program was developed as one of the practical exercises in a **Real-Time Embedded Systems** course.

The purpose of the exercise was to become familiar with local IPC using Unix domain sockets and the socket programming lifecycle rather than to implement a production networking framework.

The original coursework version is preserved in the repository under the `coursework-submission` Git tag.

## Limitations

This is intentionally a small educational implementation.

In particular:

- communication is limited to processes on the same host
- clients are handled sequentially rather than concurrently
- the protocol contains only two commands
- the application uses a simple text-based request/response model
- it is intended to demonstrate Unix socket fundamentals rather than provide a reusable IPC library