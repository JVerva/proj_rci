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

//checks if commands are valid, returns comand index if they are, returns -1 and prints error msg if they aren't
//args gets filled with arguments for command
int commandcheck(char buffer[], char** args);

//joins node to network. if node id is already in the networks, use a new free one.
int join(int fd_udp, int fd_tcp, struct node_info* nodeinfo, char net[], char id[], char ip[], char tcp[], struct addrinfo serverinfo, fd_set* rfds);

//joins node to network.
int djoin(int fd_tcp, int fd_udp, struct node_info* nodeinfo, char net[], char id[], char ip[], char tcp[], char bootid[], char bootip[], char boottcp[], struct addrinfo serverinfo,fd_set* rfds);

//unregisters node from network
int leave(int udp, int tcp, char net[], char id[],struct node_info** node_info,  struct addrinfo serverinfo, fd_set* rfds);

//print to terminal the network topology
int show_topology(struct node_info* node);

//execute the user command "get"
int get(struct node_info* nodeinfo, char dest[], char name[]);