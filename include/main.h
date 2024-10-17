#ifndef MAIN_H
#define MAIN_H

struct configuration {
  int PORT;
};

void sigint_handler(int sig);

char *strip_uri(char *uri);

void respond(const char *file_path);

void init (struct sockaddr_in *hostaddr, size_t hostaddrlen);


#endif // !MAIN_H
