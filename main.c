#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int initTcpSocket(char [], char[]);
int initUdpSocket(char [], char[]);
//a

int main(int argc, char* argv[]){

    //get arguments
    if(argc != 5){
        fprintf(stderr, "wrong number of arguments: %d.\n", argc);
        exit(1);
    }

    char *ip = argv[1];
    char *port = argv[2];
    char *regIp = argv[3];
    char *regPort = argv[4];

    //init tcp socket
    int tcpFd = initTcpSocket(ip, port);
    //init udp sockt to comunicate with network server
    int udpFD = initUdpSocket(ip, port);


    //file descriptor set
    fd_set rfds;
    //number of ready file descriptors
    int counter;
    char buffer[128];

    //program loop
    while(1){

        //reset file descriptor set
        FD_ZERO(&rfds);
        //add keyboard to file descriptor set
        FD_SET(0, &rfds);
        //add tcp socket to file descriptor set
        FD_SET(tcpFd, &rfds);

        counter = select(tcpFd+1, &rfds, NULL, NULL, NULL);

        if(counter<=0){
            fprintf(stderr, "select error: %d.\n", counter);
            exit(1);
        }
        else{
            //bytes read
            int n;
            //keyboard ready
            if(FD_ISSET(0, &rfds)){
                FD_CLR(0,&rfds);
                n = read(0,buffer,128);
                printf("keyboard: %s\n",buffer);
            }
            //tcp socket ready
            if(FD_ISSET(tcpFd, &rfds)){
                FD_CLR(tcpFd,&rfds);
                n = read(0,buffer,128);
                printf("socket: %s\n", buffer);
            }
        }
    }
    return 0;
}

//create a tcp socket, bind to port and listen for incoming connections
int initTcpSocket(char ip[], char port[]){
    int status;
    struct addrinfo hints, *res;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //get address info
    if(status = getaddrinfo(ip, port, &hints, &res) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    //bind socket to port
    bind(sockfd, res->ai_addr, res->ai_addrlen);

    //listen for incoming connections
    listen(sockfd, 5);

    return sockfd;
}

//create a udp socket
int initUdpSocket(char ip[], char port[]){
    int status;
    struct addrinfo hints, *res;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //get address info
    if(status = getaddrinfo(ip, port, &hints, &res) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    return sockfd;
}