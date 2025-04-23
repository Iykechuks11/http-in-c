// HTTP server in C
// The evolution of the HTTP - https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Evolution_of_HTTP

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h> // For perror
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> // For close
#include <string.h> // For memset

#define PORT 6969 // Port number for the server

int handle_client(int client_fd) // Handles client requests
{
  ssize_t n = 0;
  char buf[1024]; // Buffer to hold the received data
  // In the HTTP protocol, the CR-LF sequence is always used to terminate a line.
  // The CR (Carriage Return) character is represented by '\r' and the LF (Line Feed) character is represented by '\n'.
  // The CR-LF sequence is used to indicate the end of a line in HTTP headers and messages.
  // The HTTP protocol uses the CR-LF sequence to separate headers and to indicate the end of a message.
  const char* hello = "HTTP/1.1 200 OK\r\n\r\n<h1>Hello, World!</h1>";

  printf("\n---------------------\n");
  for (;;) 
  {
    memset(buf, 0, sizeof(buf)); // Initialize the buffer to zero
    
    // Read data from the client
    n = read(client_fd, buf, sizeof(buf) - 1);
    if (n < 0)
    {
      perror("read(client)");
      return -1; // Return -1 on error
    }
    if (n == 0)
    {
      printf("Client disconnected\n");
      return 0; // Return 0 if the client disconnected
    }
  
    printf("Received %zd bytes: %s\n", n, buf); // Print the received data
    // Send a response to the client
    // The write() function sends data to the client.
    (void)write(client_fd, hello, strlen(hello)); // Send the HTTP response
    printf("Sent %zd bytes: %s\n", strlen(hello), hello); // Print the sent data
    close(client_fd); // Close the client socket
    break; // Exit the loop after handling the client
    
  }
    
  printf("\n---------------------\n");
  printf("Client connection closed\n");
  return 0; // Return 0 on success
};

int main(void) 
{
  // Declaration of variables
  int rc = 0;
  int server_fd = 0;
  struct sockaddr_in bind_addr = {0};
  int client_fd;
  int ret = 0;
  int resuse = 1; // Variable to set socket options

  // Step 1: Create a socket
  // The socket() function creates a new socket and returns its file descriptor.
  // Documentation from Archlinux - https://man.archlinux.org/man/tcp.7
  memset(&bind_addr, 0, sizeof(bind_addr)); // Initialize the bind_addr structure to zero
  server_fd = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // TCP
      0            // Default protocol
  );

  // Check if the socket was created successfully
  if (server_fd == -1) 
  {
    perror("socket");
    ret = 1;
    goto exit; // Exit if socket creation failed
  }

  printf("\n---------------------\n");
  printf("Socket created successfully\n");

  // Set socket options (optional)
  // The setsockopt() function is used to set options on the socket.
  // The SO_REUSEADDR option allows the socket to bind to an address that is already in use.
  // This is useful for allowing the server to restart quickly after a crash.
  // Documentation from Archlinux - https://man.archlinux.org/man/core/man-pages/setsockopt.2.en
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &resuse, sizeof(resuse)) == -1) 
  {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    ret = 1;
    goto exit; // Exit if setsockopt failed
  }

  // Step 2: Set up the address structure (bind address)
  // The sockaddr_in structure is used to specify an endpoint address for IPv4.
  // It contains the port number, address family, and IP address.
  // The sockaddr_in structure is defined in <netinet/in.h>.
  // Documentation from Archlinux - https://man.archlinux.org/man/sockaddr_in.3type
  bind_addr.sin_family = AF_INET;               // Address family (IPv4)
  bind_addr.sin_port = htons(PORT);             // Port number (convert to network byte order)
  bind_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces

  // Step 3: Bind the socket to the address
  rc = bind(server_fd, (const struct sockaddr *)&bind_addr, sizeof(bind_addr));

  // Check if the bind was successful
  if (rc == -1) 
  {
    perror("bind()");
    ret = 1;
    goto exit; // Exit if bind failed
  }

  printf("Socket bound to port %d\n", PORT);

  // Step 4: Listen for incoming connections
  // The listen() function marks the socket as a passive socket that will be used to accept incoming
  // connection requests. The backlog parameter specifies the maximum length of the queue for pending
  // connections. If the queue is full, incoming connection requests may be refused or ignored.
  // Documentation from Archlinux - https://man.archlinux.org/man/core/man-pages/listen.3p.en
  // SOMAXCONN is a constant that specifies the maximum length of the queue for pending connections.
  // It is defined in <sys/socket.h>. short for Socket Outstanding MAX CONNections.
  rc = listen(server_fd, SOMAXCONN);
  // Check if the listen was successful
  if (rc == -1) {
    perror("listen()");
    ret = 1;
    goto exit; // Exit if listen failed
  }

  printf("Listening for incoming connections...\n");

  // Step 5: Accept incoming connections
  for (;;) {
    printf("Waiting for a connection...\n");
    // The accept() function extracts the first connection request on the queue of pending connections
    // and creates a new socket for the connection.
    // It blocks the calling thread until a connection is present.
    // Documentation from Archlinux - https://man.archlinux.org/man/core/man-pages/accept.3p.en
    // The client_fd variable will hold the file descriptor for the accepted connection.
    client_fd = accept(server_fd, NULL, NULL);
    // Check if the accept was successful
    printf("Accepted connection\n");

    rc = handle_client(client_fd); // Handle the client connection
    
    if (rc == -1) {
      perror("handle_client");
      ret = 1;
      goto exit; // Exit if handle_client failed
    }
    printf("Client handled successfully\n");
  };

exit:
  close(server_fd); // Close the socket after use
  return ret;
  // Note: In a real server, you would typically enter a loop here to accept incoming connections
}