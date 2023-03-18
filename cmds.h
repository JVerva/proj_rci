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

int commandcheck(char[], char*[]);
int join(int, int, char[] , char[], char[], char[], struct addrinfo);
int djoin(int, int, char[] , char[], char[], char[],char[], struct addrinfo);
int leave(int, char[], int, struct addrinfo);