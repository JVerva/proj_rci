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
#include "contact.h"
#include "node_protocols.h"
#include "cmds.h"
#include "utils.h"

int inittcpsocket(char[]);
int initudpsocket(char[], char[], struct addrinfo**);
int checkfornode(char[], char[]);

char DEFAULT_IP[] = {"193.136.138.142"};
char DEFAULT_PORT[] = {"59000"};

int main(int argc, char* argv[]){
    char *net;
    char *id = {"-1"};
    char *reg_ip = (char*)malloc(sizeof(char*));
    char *reg_port = (char*)malloc(sizeof(char*));
    Contact contact_head=NULL;
    Contact contact_aux=NULL;
    int joined = 0;
    //get arguments
    if(argc != 5 && argc != 3){
        fprintf(stderr, "wrong number of arguments: %d.\n", argc);
        exit(1);
    }if (argc == 5){
        reg_ip = argv[3];
        reg_port = argv[4];
    }else if(argc == 3){
        strcpy(reg_ip, DEFAULT_IP);
        strcpy(reg_port, DEFAULT_PORT);
    }

    char *ip = argv[1];
    char *port = argv[2];

    struct addrinfo* node_server = (struct addrinfo*)malloc(sizeof(struct addrinfo*));

    //init tcp socket
    int fd_tcp = inittcpsocket(port);
    //init udp sockt to comunicate with network server
    int fd_udp = initudpsocket(reg_ip, reg_port, &node_server);

    //Node information
    struct node_info *node_info;

    //file descriptor set
    fd_set rfds, aux_rfds;
    //number of ready file descriptors
    int counter;
    char buffer[128];

    //reset file descriptor set
    FD_ZERO(&rfds);
    //add keyboard to file descriptor set
    FD_SET(0, &rfds);
    //add tcp socket to file descriptor set
    FD_SET(fd_tcp, &rfds);

    //program loop
    while(1){

        //use auxiliar as not to set everything every iteration
        aux_rfds = rfds;

        counter = select(fd_tcp+10, &aux_rfds, NULL, NULL, NULL);

        if(counter<=0){
            fprintf(stderr, "select error: %d.\n", counter);
            exit(1);
        }
        else{
            //bytes read
            int n;
            //keyboard ready
            if(FD_ISSET(0, &aux_rfds)){
                n = read(0,buffer,128);
                if(n == -1){
                    fprintf(stderr, "read error.\n");
                }
                char **args = (char**)malloc(6*sizeof(char**));
                int cmd = commandcheck(buffer, args);
                switch(cmd){
                    case -1:
                        //error on commands (skip)
                        break;
                    case 0:
                        //join
                        if(joined == 0){
                            id = strdup(args[1]);
                            net = strdup(args[0]);
                            join(fd_udp,fd_tcp, net, id, ip, port, *node_server)
                            //Snode_info = initNode_info(id);
                            joined = 1;
                        }else{
                            fprintf(stderr,"already joined the network as node %s.\n", id);
                        }
                        break;
                    case 1:
                        //djoin
                        djoin(fd_udp,fd_tcp, args[0], args[1], args[2], args[3], args[4], *node_server);
                        node_info = initNode_info(id);
                        break;
                    case 9:
                        //leave
                        if(joined == 0){
                            fprintf(stderr,"node not registered.\n");
                        }else{
                            leave(fd_udp, net, atoi(id), *node_server);
                            //reset variables
                            closeNode_info(node_info);
                            joined = 0;
                        }
                        break;
                    case 10:
                        //exit
                        return 0;
                    default:
                        fprintf(stderr, "command not yet implemented.\n");
                        break;
                }
                free(args);
            }
            //tcp socket ready
            if(FD_ISSET(fd_tcp, &aux_rfds)){
                //create new fd to deal with message
                int new_fd;
                struct sockaddr* addr;
                socklen_t addr_size = sizeof(addr);
                if((new_fd = accept(fd_tcp, (struct sockaddr*) &addr, &addr_size)) == -1){
                    fprintf(stderr, "error accepting new connection.\n");
                }else{
                    printf("accepted connection.\n");
                    //add file descriptor to select checks
                    FD_SET(new_fd, &rfds);
                    //create new contact with default data (to be filled later)
                    contact_head = addContact(contact_head, "-1", new_fd);
                }
            }
            //any neighbor ready
            contact_aux = contact_head;
            
            while(contact_aux != NULL){
                if(FD_ISSET(contact_aux->fd, &aux_rfds)){
                    n = read(contact_aux->fd,buffer,128);
                    if(n == -1){
                        fprintf(stderr, "read error.\n");
                    }
                    char **args = (char**)malloc(3*sizeof(char**));
                    int msg = messagecheck(buffer, args);
                    switch(msg){
                        case 0:
                            //new
                            if(fillContact(contact_aux, args[0], args[1], args[2])==0){
                                printf("added new contact, %s.\n", args[0]);
                            }else{
                                fprintf(stderr, "failed to add new contact.\n");
                            }
                        break;
                        case -1:
                            //error
                            fprintf(stderr, "error: message does not correspond to protocol.\n");
                        break;
                    }
                    free(args);
                }
                contact_aux = contact_aux->next;
            }
            memset(buffer,0,sizeof(buffer));
        }

    }
    close(fd_tcp);
    close(fd_udp);
    return 0;
}

//create a tcp socket, bind to port and listen for incoming connections
int inittcpsocket(char port[]){
    int status;
    struct addrinfo hints, *res;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //get address info
    if((status = getaddrinfo(NULL, port, &hints, &res)) != 0){
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
int initudpsocket(char ip[], char port[], struct addrinfo **res){
    int status;
    struct addrinfo hints;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //get address info
    if((status = getaddrinfo(ip, port, &hints, res)) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd = socket((*res)->ai_family, (*res)->ai_socktype, (*res)->ai_protocol);

    return sockfd;
}
