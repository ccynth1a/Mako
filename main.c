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

#define PORT 8080 //Defines the port being used as the tcp port
#define BUFFER_SIZE 1024 //Buffer used throughout, so standardizing the value here

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//Global Vars
int socket_file_descriptor = 0, connection_socket_file_descriptor = 0; //Set them to zero to flag them as uninitialised for the next function

//When SIGINT is called, close all open File Descriptors and exit the program
void sigint_handler(int sig)
{
  int status1, status2;
  if (socket_file_descriptor == 0) {
    status1 = close(socket_file_descriptor);
  } 
  if (connection_socket_file_descriptor == 0) {
    status2 = close(connection_socket_file_descriptor);
  }
  if (status1 == -1 || status2 == -1) {
    char *error_msg;
    sprintf(error_msg, "%sERROR: (CLOSE)%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    perror(error_msg);
    exit(1);
  }
  printf("\nSIGINT, files closed successfully\n");
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
    printf("%sERROR: (FILE NOT FOUND)\n%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    const char *header =  "HTTP/1.0 404 Not Found\r\n"
                          "Server: webserver-c\r\n"
                          "Content-type: text/html\r\n";
    const char *body = "<html><body>404 Not Found</body></html>\r\n";
    write(connection_socket_file_descriptor, header, strlen(header));
    write(connection_socket_file_descriptor, body, strlen(body));
  } 
  // Figuring out the content type
  const char *file_extension = strrchr(file_path, '.'); //Extract the file extension
  const char *content_type;
  if (strcmp(file_extension, ".html") == 0) { // TODO: Make this NOT a mess. For now its ordered from what's probably most-least common
    content_type = "text/html";
  } else if (strcmp(file_extension, ".css") == 0) {
    content_type = "text/css";
  } else if (strcmp(file_extension, ".js") == 0) {
    content_type = "application/javascript";
  } else if (strcmp(file_extension, ".jpg") == 0 || strcmp(file_extension, ".jpeg") == 0) {
    content_type = "image/jpeg"; 
  } else if (strcmp(file_extension, ".png") == 0) {
    content_type = "image/png"; 
  } else if (strcmp(file_extension, ".mp4") == 0) {
    content_type = "video/mp4";
  } else if (strcmp(file_extension, ".mp3") == 0) {
    content_type = "video/mpeg";
  } else if (strcmp(file_extension, ".webm") == 0) {
    content_type = "video/webm";
  } else {
    content_type = "application/octet-stream"; //Default to binary stream if no appropriate delivery method
  }
  // File has been found successfully and content type has been worked out, create the header and prepare to send the file
  char header[BUFFER_SIZE];
  snprintf(header, sizeof(header), 
                        "HTTP/1.0 200 OK\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: %s\r\n\r\n", content_type);
  write(connection_socket_file_descriptor, header, strlen(header));
  
  char buffer[BUFFER_SIZE]; //Create buffer for sending the data
  ssize_t bytes_r, bytes_s; //Bytes read and sent
  while ((bytes_r = read(file_descriptor, buffer, sizeof(buffer))) > 0) { //Will continue as long as there are bytes to send
    bytes_s = write(connection_socket_file_descriptor, buffer, bytes_r); //Write the contents of the file
    if (bytes_s < 0) {
      char *error_msg;
      sprintf(error_msg, "%sERROR: (WRITE)%s", ANSI_COLOR_RED, ANSI_COLOR_RESET); //format error message
      perror(error_msg);
      close(file_descriptor);
      return;
    }
  }
  printf("%sFinished serving %s, closing file descriptor%s\n", ANSI_COLOR_GREEN, file_path, ANSI_COLOR_RESET);
  close(file_descriptor);
}

//Define host protocol information, bind, and listen
void init(struct sockaddr_in* host_addr, size_t host_addrlen) 
{
  //Define information relating to the host machine
  host_addr->sin_family = AF_INET;
  host_addr->sin_port = htons(PORT); //Converts to network port order 
  host_addr->sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
  // INADDR_ANY is just 0.0.0.0, ie all local IP addresses

  //Bind the socket 
  if (bind(socket_file_descriptor, (struct sockaddr*)host_addr, host_addrlen) != 0) {
      char *error_msg;
      sprintf(error_msg, "%sERROR: (SOCKET BIND)%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
      perror(error_msg);
      exit(1);
  }
  printf("%sBound on Port 8080\n%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
                                                                                                                                         
  if (listen(socket_file_descriptor, SOMAXCONN) != 0) { //SOMAXCONN is the max number of connections that can be queued, default is 128 
    char *error_msg;
    sprintf(error_msg, "%sERROR: (LISTEN)%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    perror(error_msg);
    exit(1);
  }
  //Now that we're listening, potential connections will build up in a queue
  printf("%sListening on Port 8080...\n%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
}

int main(int argc, char *argv[])
{
  //Create the socket file descriptor
  //File descriptor: Integer identifier to a file
  socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0); //Create IPv4 TCP Socket
  if (socket_file_descriptor != -1) {
    printf("%sSocket file descriptor created successfully\n%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
  } else {
    char *error_msg;
    sprintf(error_msg, "%sERROR: (SOCKET FD CREATE)%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    perror(error_msg);
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
    //Create a new socket to accept the incoming connection
    connection_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr*)&host_addr, (socklen_t *)&host_addrlen);
    if (connection_socket_file_descriptor < 0) {
      char *error_msg;
      sprintf(error_msg, "%sERROR: (CONNECTION ACCEPT)\n%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
      perror(error_msg);
      //Reset the loop and handle the next client
      continue;
    }
    printf("%sAccepted TCP Connection\n%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);

    //Get client information before reading
    int socket_name = getsockname(connection_socket_file_descriptor, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
    //It might be empty for some reason
    if (socket_name < 0) {
      char *error_msg;
      sprintf(error_msg, "%sERROR: (SOCKET NAME)\n%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
      perror(error_msg);
    }

    //Now time to read the request
    char buffer[BUFFER_SIZE];

    //If the read() function returns a value less than zero, it's fucked up, but like what's the worst that could happen
    int read_status = read(connection_socket_file_descriptor, buffer, BUFFER_SIZE);

    //Parse request 
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE]; //Method is GET, uri is the file requested, version is obviously the HTTP version
    sscanf(buffer, "%s %s %s", method, uri, version);
    //Give information about the client
    printf("IP: %s\nPort: %u\n"
           "%s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    if (read_status < 0) {
      char *error_msg;
      sprintf(error_msg, "%sERROR: (READ REQUEST)\n%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
      perror(error_msg);
      continue; //If i cant read your request, you're getting skipped
    }
 
    //TIME TO GET THE FILE PATHHHHH!!!!
    if (strcmp(uri, "/") == 0) {
      respond("index.html");
    } else { 
      char *stripped_uri = strip_uri(uri);
      respond(stripped_uri);
      free(stripped_uri);
    } 
    close(connection_socket_file_descriptor);
  }

  //Close the socket
  close(socket_file_descriptor);
  return 0;
}

