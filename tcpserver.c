#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define PORT "59001"

int main(){

    int status;
    struct addrinfo hints, *res;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //get address info
    if((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    //bind socket to port
    bind(sockfd, res->ai_addr, res->ai_addrlen);

    //listen for incoming connections
    listen(sockfd, 5);

    int status_udp;
    struct addrinfo hints_udp;
    int sockfd_udp;

    //fill hints
    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;

    //get address info
    if((status = getaddrinfo("193.136.138.142", "59000", &hints_udp, &res)) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd_udp = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    char cmd[40] = {"\0"};
    char buff[256] = {"\0"};
    memset(buff,0,256);
    memset(cmd,0,40);
    sprintf(cmd, "REG %s %s %s %s", "032", "00", "192.168.1.100", PORT);
    int n = sendto(sockfd_udp, cmd , 40 ,0,res->ai_addr, res->ai_addrlen);
    printf("%d\n",n);

    while(1){

    }
    return 0;
}