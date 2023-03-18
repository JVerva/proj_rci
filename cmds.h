#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "utils.h"
#include "contact.h"
#include "node_protocols.h"

#define YEL "\x1b[33m"
#define WHI "\x1b[37m"
#define NORM "\x1b[m"
#define BOLD  "\e[1m"
#define NBOLD   "\e[m"

int commandcheck(char[], char*[]);
int join(int, int, char[] , char[], char[], char[], struct addrinfo);
int djoin(int, int, char[] , char[], char[], char[],char[], struct addrinfo);
int leave(int, char[], int, struct addrinfo);
int show_topology(struct node_info* node);