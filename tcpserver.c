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

#define PORT "59002"

int main(){

    int newfd, fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    fd_set rfds;

    fd=socket(AF_INET, SOCK_STREAM,0);
    if(fd==-1){
        printf("socket.\n");
         /*error*/ exit(1);
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    errcode=getaddrinfo("127.0.0.1", PORT, &hints, &res);
    if(errcode!=0){
        printf("sddrinfo.\n");
         /*error*/ exit(1);
    }

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        perror("connect.\n");
         /*error*/ exit(1);
    }

    while (1){
        scanf("%s",buffer);
        n=write(fd,buffer, 128);
        if(n==-1)/*error*/ exit(1);
    }
    

    //n=read(fd,buffer,128);
    //if(n==-1)/*error */exit(1);
    //write(1,"echo: ", 6); write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);

    return 0;

    
}