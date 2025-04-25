# Simple HTTP Server in C

This is a basic HTTP server written in C. It listens on port **6969** and responds to incoming connections with a simple HTML message:  
`<h1>Hello, World!</h1>`

## How It Works

- Creates a TCP socket using the `socket()` system call.
- Binds to port 6969 on all available interfaces.
- Listens for incoming connections using `listen()`.
- Accepts a connection with `accept()`.
- Reads the client's HTTP request with `read()`.
- Sends a basic HTTP 200 OK response with a hardcoded HTML body.
- Closes the connection after responding.

## Requirements

This program uses only standard C libraries:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
```

No external libraries or frameworks are required.

## Build and Run

To compile and run:

```bash
gcc -o http_server http_server.c
./http_server
```

Then in a browser or with `curl`:

```bash
curl http://localhost:6969
```

You should see:

```html
<h1>Hello, World!</h1>
```

## Output Example

When a client connects, the server logs the received request and the sent response:

```text
---------------------
Socket created successfully
Socket bound to port 6969
Listening for incoming connections...
Waiting for a connection...
Accepted connection
Received 123 bytes: GET / HTTP/1.1 ...
Sent 38 bytes: HTTP/1.1 200 OK...

Client connection closed
```

## Reference

- [Evolution of HTTP (MDN)](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Evolution_of_HTTP)
- Linux man pages: `socket`, `bind`, `listen`, `accept`, `read`, `write`, `close`

---

_Note: This is for educational purposes only. It does not handle concurrent connections or parse HTTP headers._
