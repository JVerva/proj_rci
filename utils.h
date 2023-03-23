#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int verifyid(char* id);
int verifynet(char* net);
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