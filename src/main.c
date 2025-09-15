/* 
 * Mako: Version 1.1.3
 * Author: CCynth1a
 * Distribution: Feel free <3
 *
*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include "../include/main.h"
#include "../include/extern_include/timber.h"

#define BUFFER_SIZE 1024 //Buffer used throughout, so standardizing the value here

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//Global Vars
static struct hash_t *hashtab[MAX_HASH_SIZE];

int socket_file_descriptor = 0, connection_socket_file_descriptor = 0; //Set them to zero to flag them as uninitialised for the next function

char snbuffer[BUFFER_SIZE]; // buffer for snprintf, overwritten constantly

// set up a configuration struct
struct configuration config;
logger_t *logger;

int main(int argc, char *argv[])
{
  // setting up timber debug logger 
  logger = (logger_t *)malloc(sizeof(logger_t));
  log_init(logger, INFO, NULL);
  
  logger->line_enabled = false;
  logger->function_enabled = false;

  // Initialise Hash Table For Content Headers
  char extensions[8][6] = {
    ".html",
    ".css",
    ".js",
    ".jpg",
    ".png",
    ".mp4",
    ".mp3",
    ".webm"
  };

  char content_type[8][32] = {
    "text/html",
    "text/css",
    "application/javascript",
    "image/jpeg",
    "image/png",
    "video/mp4",
    "video/mpeg",
    "video/webm"
  };

  for (int i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++) {
    install(extensions[i], content_type[i]);
  };

  // Set default Port to 8080
  config.PORT = 8080;

  /* ARG PARSING */ 
  if (argc >= 2) {
    while (true) {
      int opt = getopt(argc, argv, "p:v");
      if (opt == -1) 
        break;
      switch (opt) {
        case 'p':
          config.PORT = atoi(optarg);
          break;
        case 'v':
          logger->line_enabled = true;
          logger->function_enabled = true;
          break;
        default:
          LOG(logger, CRITICAL, "Bad Usage");
          exit(1);
      }
    }
  } else {
    config.PORT = 8080;
  }
  //Create the socket file descriptor
  //File descriptor: Integer identifier to a file
  socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0); //Create IPv4 TCP Socket
  int sock_opt = 1;
  setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));
  if (socket_file_descriptor != -1) {
    LOG(logger, INFO, "Socket file descriptor created successfully");
  } else {
    LOG(logger, CRITICAL, "Failed to create Socket File Descriptor");
    return 1;
  }

  signal(SIGINT, sigint_handler); //Set interrupt function to execute on SIGINT
  
  //Define a socket internet address of our host machine
  struct sockaddr_in host_addr;
  size_t host_addrlen = sizeof(host_addr);
  
  //Define a socket internet address of our client
  struct sockaddr_in client_addr;
  int client_addrlen = sizeof(client_addr);
  
  //Define info, bind, and listen
  init((struct sockaddr_in*)&host_addr, host_addrlen);

  //Now we forever assign and handle the creation of new sockets for each of these connections 
  while (true) {
    // New paragraph 
    printf("%s──────────────────────────────────────────%s\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
    //Create a new socket to accept the incoming connection
    connection_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr*)&host_addr, (socklen_t *)&host_addrlen);
    if (connection_socket_file_descriptor < 0) {
      LOG(logger, ERROR, "Failed to accept TCP Connection");
      //Reset the loop and handle the next client
      continue;
    }
    LOG(logger, INFO, "Accepted TCP Connection");

    //Get client information before reading
    int socket_name = getsockname(connection_socket_file_descriptor, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
    //It might be empty for some reason
    if (socket_name < 0) {
      LOG(logger, CRITICAL, "Invalid Socket Name");
      exit(1);
    }

    //Now time to read the request
    char buffer[BUFFER_SIZE];

    //If the read() function returns a value less than zero, it's fucked up, but like what's the worst that could happen
    int read_status = read(connection_socket_file_descriptor, buffer, BUFFER_SIZE);

    //Parse request 
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE]; //Method is GET, uri is the file requested, version is obviously the HTTP version
    sscanf(buffer, "%s %s %s", method, uri, version);
    //Give information about the client
    snprintf(snbuffer, BUFFER_SIZE, "IP: %s Port: %u\n"
           "%s %s %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
    LOG(logger, INFO, snbuffer);

    if (read_status < 0) {
      LOG(logger, ERROR, "Error reading from socket, handling next client in queue...");
      continue; //If i cant read your request, you're getting skipped
    }
 
    //TIME TO GET THE FILE PATHHHHH!!!!
    if (strcmp(uri, "/") == 0) {
      respond("index.html");
    } else { 
      char *stripped_uri = strip_uri(uri);
      snprintf(snbuffer, BUFFER_SIZE, "Stripped URI: %s", stripped_uri);
      LOG(logger, DEBUG, snbuffer);
      respond(stripped_uri);
      free(stripped_uri);
    } 
    close(connection_socket_file_descriptor);
  }

  //Close the socket
  close(socket_file_descriptor);

  // Free the memory allocated to timber logger 
  log_kill(logger);
  return 0;
}

//When SIGINT is called, close all open File Descriptors and exit the program
void sigint_handler(int sig)
{
  int status1, status2;
  if (socket_file_descriptor != 0) {
    status1 = close(socket_file_descriptor);
  } 
  if (connection_socket_file_descriptor != 0) {
    status2 = close(connection_socket_file_descriptor);
  }
  /* NOTE: THIS PART HAS BEEN COMMENTED OUT AS IT IS ACTUALLY EXPECTED BEHAVIOUR.
  if (status1 == -1 || status2 == -1) {
    LOG(log, CRITICAL, "Failed to close socket file descriptor.");
    exit(1);
  } */
  LOG(logger, INFO, "SIGINT, files closed successfully");
  fflush(stdout);
  exit(0);
} 

// Is this yuri?????
char *strip_uri(char *uri)
{
  size_t len = strlen(uri);
  char *stripped_uri = malloc(len);
  for (int i = 1;i < len;i++) {
    stripped_uri[i-1] = uri[i];
  }
  stripped_uri[len-1] = '\0';
  return stripped_uri;
}

//Response to the HTTP request. This is a monolithic function that does a lot but it can kinda be broken down into:
// - Open the file 
// - Check it actually exists 
// - Read file extension, then formulate a header 
// - Funky shit sending the bytes 
// - Close the file descriptor
void respond(const char *file_path)
{
  int file_descriptor = open(file_path, O_RDONLY); //Open a file in read only mode
  if (file_descriptor == -1) { //If Error, respond with 404, since its just the file not existing
    LOG(logger, ERROR, "File not found, sending 404 response...");
    const char *header =  "HTTP/1.0 404 Not Found\r\n"
                          "Server: webserver-c\r\n"
                          "Content-type: text/html\r\n";
    const char *body = "<html><body>404 Not Found</body></html>\r\n";
    
    snprintf(snbuffer, BUFFER_SIZE, "%s%s", header, body);
    LOG(logger, DEBUG, snbuffer);
    write(connection_socket_file_descriptor, header, strlen(header));
    write(connection_socket_file_descriptor, body, strlen(body));
  } 
  // Figuring out the content type
  const char *file_extension = strrchr(file_path, '.'); //Extract the file extension

  // guards against files without an extension 
  if (file_extension == NULL) {
    return;
  }

  // I FINALLY FIXED IT BITCHES
  const char *content_type;
  struct hash_t *hp = lookup(file_extension);
  if (hp == NULL || hp->defn == NULL) {
    content_type = "application/octet-stream";
  } else {
    content_type = hp->defn;
  }

  // File has been found successfully and content type has been worked out, create the header and prepare to send the file
  char header[BUFFER_SIZE];
  snprintf(header, BUFFER_SIZE, 
                        "HTTP/1.0 200 OK\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: %s\r\n\r\n", content_type);
  // Purely for clarity when printing, we use a different buffer for logging. This keeps each request isolated to a specific "paragraph"
  snprintf(snbuffer, BUFFER_SIZE,
                        "HTTP/1.0 200 OK\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: %s", content_type);
  LOG(logger, INFO, snbuffer);
  write(connection_socket_file_descriptor, header, strlen(header));
  
  char buffer[BUFFER_SIZE]; //Create buffer for sending the data
  ssize_t bytes_r, bytes_s; //Bytes read and sent
  while ((bytes_r = read(file_descriptor, buffer, BUFFER_SIZE)) > 0) { //Will continue as long as there are bytes to send
    bytes_s = write(connection_socket_file_descriptor, buffer, bytes_r); //Write the contents of the file
    if (bytes_s < 0) {
      LOG(logger, ERROR, "Unable to write to socket, closing file descriptor...");
      close(file_descriptor);
      return;
    }
  }
  snprintf(snbuffer, BUFFER_SIZE, "Finished serving %s, closing file descriptor and request", file_path);
  LOG(logger, INFO, snbuffer);
  close(file_descriptor);
}

//Define host protocol information, bind, and listen
void init(struct sockaddr_in *host_addr, size_t host_addrlen) 
{
  //Define information relating to the host machine
  host_addr->sin_family = AF_INET;
  host_addr->sin_port = htons(config.PORT); //Converts to network port order 
  host_addr->sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
  // INADDR_ANY is just 0.0.0.0, ie all local IP addresses

  //Bind the socket 
  if (bind(socket_file_descriptor, (struct sockaddr*)host_addr, host_addrlen) != 0) {
      LOG(logger, CRITICAL, "Unable to bind to socket");
      exit(1);
  }
  snprintf(snbuffer, BUFFER_SIZE, "Bound on Port %d", config.PORT);
  LOG(logger, INFO, snbuffer);
                                                                                                                                         
  if (listen(socket_file_descriptor, SOMAXCONN) != 0) { //SOMAXCONN is the max number of connections that can be queued, default is 128 
    LOG(logger, CRITICAL, "Unable to listen on bound socket");
    exit(1);
  }
  //Now that we're listening, potential connections will build up in a queue
  snprintf(snbuffer, BUFFER_SIZE, "Listening on Port %d...", config.PORT);
  LOG(logger, INFO, snbuffer);
};

unsigned int hash(const char *s)
{
  unsigned int hashval = 0;
  for (; *s != '\0'; s++) {
    hashval = *s + 31 * hashval;
  }
  return hashval % MAX_HASH_SIZE;
};

struct hash_t *lookup(const char *s)
{
  struct hash_t *hp;
  for (hp = hashtab[hash(s)]; hp != NULL; hp = hp->next) {
    if (strcmp(s, hp->name) == 0) {
      return hp;
    }
  }
  return NULL;
};

struct hash_t *install(const char *name, const char *defn)
{
  struct hash_t *hp;
  unsigned int hashval;
  if ((hp = lookup(name)) == NULL) {
    hp = malloc(sizeof(*hp));
    if (hp == NULL || (hp->name = strdup(name)) == NULL) {
      return NULL;
    }
    hashval = hash(name);
    hp->next = hashtab[hashval];
    hashtab[hashval] = hp;
  } else {
    free(hp->defn);;
  }
  if ((hp->defn = strdup(defn)) == NULL) {
    return NULL;
  }
  return hp;
}
