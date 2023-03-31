#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

//check if id is in correct format
int verifyid(char* id);

//check if net is in correct format
int verifynet(char* net);

//check if node already exists in network
//node list is the list of nodes returned by network
//returns 1 if it already exists
int checkfornode(char node_id[], char node_list[]);

//check if there are existing nodes in the network
int isnetworkempty(char node_list[]);

int getnodeinfo(char node_id[], char node_list[], char ip[], char port[]);

//returns a tcp socket connected to input adress
int connecttonode(char *t_ip, char *t_port);

//gets nodelist from network
int getnodelist(int fd_udp,struct addrinfo serverinfo, char* net, char node_list[]);

//registers node in network
int registernode(int fd_udp,struct addrinfo serverinfo, char* net, char* id, char* ip, char* tcp);

//unregisters node in network
int unregisternode(int fd_udp,struct addrinfo serverinfo, char* net, char* id);

//sets a time out on a file descriptor, reads and resets timeout, returns read return value
int readwithtimeout(int fd, int time, char buff[], int size);

//choose random node
int chooserandnode(char node_list[], char node_id[]);