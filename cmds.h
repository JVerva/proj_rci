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
#include "node_protocols.h"
#include "utils.h"

#define YEL "\x1b[33m"
#define WHI "\x1b[37m"
#define NORM "\x1b[m"
#define BOLD  "\e[1m"
#define NBOLD   "\e[m"

int commandcheck(char[], char*[]);
int join(int, int, struct node_info*, char[] , char[], char[], char[], struct addrinfo, fd_set*);
int djoin(int, struct node_info*, char[] , char[], char[], char[],char[], struct addrinfo);
//unregisters node from network
int leave(int udp, int tcp, char net[], char id[],struct node_info** node_info,  struct addrinfo serverinfo, fd_set* rfds);
int show_topology(struct node_info* node);
int get(struct node_info* nodeinfo, char dest[], char name[]);