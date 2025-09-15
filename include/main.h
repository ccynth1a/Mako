#ifndef MAIN_H
#define MAIN_H
#include <stdlib.h>

struct configuration {
  int PORT;
};

// all the hashing stuff goes here, shamelessly stolen from C PL lol
#define MAX_HASH_SIZE 53 // were never gonna need more than this tbh
struct hash_t {
  struct hash_t *next;
  char *name;
  char *defn;
};

unsigned int hash(const char *s);

struct hash_t *lookup(const char *s);

struct hash_t *install(const char *name, const char *defn);

void sigint_handler(int sig);

char *strip_uri(char *uri);

void respond(const char *file_path);

void init (struct sockaddr_in *hostaddr, size_t hostaddrlen);


#endif // !MAIN_H
