#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include "cmds.h"
#include "utils.h"

//create a tcp socket, bind to port and listen for incoming connections
//input is the desired tcp port
//returns the fd for the created socket
int inittcpsocket(char port[]);

//create an udp socket 
//input is the desired udp port
//returns the fd for the created socket
int initudpsocket(char[], char[], struct addrinfo**);

//handle any possible interaction with an estabilished neighbor
int handlecontact(Contact contact, struct node_info* node_info, fd_set* aux_rfds, fd_set* rfds);

//accept new connections and setup the new neighbor status
int checknewconnection(int fd_tcp, struct node_info* node_info, fd_set* aux_rfds, fd_set* rfds);

//receive data from socket and separate into messages ending in \n
int process_stream(char* stored, char* msg);


char DEFAULT_IP[] = {"193.136.138.142"};
char DEFAULT_PORT[] = {"59000"};

int main(int argc, char* argv[]){
    //ignore sigpipe errors
    struct sigaction act;
    memset(&act,0,sizeof act);
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act,NULL);
    char net[4] = {"-1"};
    char id[3] = {"-1"};
    char *reg_ip;
    char *reg_port;
    Contact contact_aux = NULL;
    Contact contact_temp = NULL;
    int joined = 0;
    srand(time(NULL));

    //get arguments
    if(argc != 5 && argc != 3){
        fprintf(stderr, "wrong number of arguments: %d.\n", argc);
        exit(1);
    }if (argc == 5){
        reg_ip = argv[3];
        reg_port = argv[4];
    }else if(argc == 3){
        reg_ip = DEFAULT_IP;
        reg_port = DEFAULT_PORT;
    }

    char *ip = argv[1];
    char *port = argv[2];

    struct addrinfo* node_server;

    //init udp sockt to comunicate with network server
    int fd_udp = initudpsocket(reg_ip, reg_port, &node_server);
    //tcp file descriptor variable
    int fd_tcp;

    //Node information
    struct node_info *node_info = initNode_info();

    //file descriptor set
    fd_set rfds, aux_rfds;
    //number of ready file descriptors
    int counter;
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));

    //reset file descriptor set
    FD_ZERO(&rfds);
    //add keyboard to file descriptor set
    FD_SET(0, &rfds);

    //program loop
    while(1){

        //use auxiliar as not to set everything every iteration
        aux_rfds = rfds;

        counter = select(FD_SETSIZE, &aux_rfds, NULL, NULL, NULL);

        if(counter<=0){
            perror("select error");
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
                char **args = (char**)malloc(6*sizeof(char*));
                int cmd = commandcheck(buffer, args);
                switch(cmd){
                    case -1:
                        //error on commands (skip)
                    break;
                    case 0:
                        //join
                        if(joined == 0){
                            //init tcp socket
                            fd_tcp = inittcpsocket(port);
                            //add tcp socket to file descriptor set
                            FD_SET(fd_tcp, &rfds);
                            strcpy(id, args[1]);
                            strcpy(net, args[0]);
                            if(join(fd_udp,fd_tcp, node_info, net, id, ip, port, *node_server, &rfds)==0){
                                joined = 1;
                            }else{
                                FD_CLR(fd_tcp, &rfds);
                            }
                        }
                        else{
                            fprintf(stderr,"already joined the network as node %s.\n", id);
                        }
                    break;
                    case 1:
                        if(joined == 0){
                            //init tcp socket
                            fd_tcp = inittcpsocket(port);
                            //add tcp socket to file descriptor set
                            FD_SET(fd_tcp, &rfds);
                            //djoin
                            strcpy(id, args[1]);
                            strcpy(net, args[0]);
                            if(djoin(fd_tcp, fd_udp, node_info, net, id, ip ,port, args[2], args[3], args[4], *node_server, &rfds)==0){
                                joined = 1;
                            }else{
                                FD_CLR(fd_tcp, &rfds);
                            }
                        }else{
                            fprintf(stderr,"already joined the network as node %s.\n", id);
                        }
                    break;
                    case 2:
                        //create name
                        node_info->names = addName(node_info->names, args[0]);
                    break;
                    case 3:
                        //delete name
                        node_info->names = removeName(node_info->names, args[0]);
                    break;
                    case 4:
                        //get
                        if(joined ==1){
                            get(node_info, args[0], args[1]);
                        }else{
                            fprintf(stderr,"node not registered.\n");
                        }
                    break;
                    case 6:
                        //show topology
                        show_topology(node_info);
                    break;
                    case 7:
                        //show names
                        show_names(node_info->names);
                    break;
                    case 8:
                        //show routing
                        show_routing(node_info->rout_table);
                    break;
                    case 9:
                        //leave
                        if(joined == 0){
                            fprintf(stderr,"node not registered.\n");
                        }else{
                            FD_CLR(fd_tcp, &rfds);
                            FD_CLR(node_info->ext->fd, &rfds);
                            leave(fd_udp, fd_tcp, net, id, &node_info, *node_server, &rfds);
                            joined = 0;
                        }
                    break;
                    case 10:
                        //exit
                        close(fd_udp);
                        //free whatever is missing
                        closeNode_info(node_info);
                        freeaddrinfo(node_server);
                        free(args);
                        return 0;
                    break;
                    case 11:
                        //cr clear routing
                        closeRoutingTable(node_info->rout_table);
                        node_info->rout_table = NULL;
                    break;
                    default:
                        fprintf(stderr, "command not yet implemented.\n");
                    break;
                }
                free(args);
            }
            if(joined == 1){
                //tcp socket ready
                checknewconnection(fd_tcp, node_info, &aux_rfds, &rfds);
                //any neighbor ready
                //check extern node
                if(strcmp(node_info->ext->id, node_info->id) != 0){
                    if(handlecontact(node_info->ext, node_info, &aux_rfds, &rfds)==-2){
                        FD_CLR(fd_tcp, &rfds);
                        FD_CLR(node_info->ext->fd, &rfds);
                        leave(fd_udp, fd_tcp, net, id, &node_info, *node_server, &rfds);
                        joined = 0;
                    }
                }
                //check inter nodes
                contact_aux = node_info->intr;
                while(contact_aux != NULL){
                    contact_temp = contact_aux;
                    contact_aux = contact_aux->next;
                    handlecontact(contact_temp, node_info, &aux_rfds, &rfds);
                }
            }
            memset(buffer,0,sizeof(buffer));
        }
    }
    return 0;
}


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

    freeaddrinfo(res);

    return sockfd;
}


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

int handlecontact(Contact contact, struct node_info* node_info, fd_set* aux_rfds, fd_set* rfds){
    char buffer[MAX_MSG+1];
    int flags = 0;
    memset(buffer, 0, MAX_MSG+1);
    if(FD_ISSET(contact->fd, aux_rfds)){
        int n = recv(contact->fd,buffer,MAX_MSG+1,flags);
        if(n == -1){
            perror("read error");
        }else if(n == 0){
            //contacts socket is closed
            //take it out of routing table
            if(node_info->rout_table!=NULL){
                node_info->rout_table = removeRoute(node_info->rout_table, contact->id);
                //send withdraw msg
                if(strcmp(node_info->ext->id,contact->id)!=0){
                    withdraw_send(node_info->ext->fd,contact->id);
                }
                for(Contact aux = node_info->intr; aux != NULL; aux = aux->next){
                    if(strcmp(aux->id, contact->id)!=0){
                        withdraw_send(aux->fd, contact->id);
                    }
                }
            }
            //if it is extern contact, connect to backup
            if(strcmp(node_info->ext->id,contact->id)==0){
                FD_CLR(node_info->ext->fd, rfds);
                node_info->ext = removeContact(node_info->ext, contact);
                node_info->ext = createContact();
                //if back up is myself(am anchor), choose another intern node to be extern
                if(strcmp(node_info->bck->id, node_info->id)==0){
                    //check if there are intern neighbors
                    if(node_info->intr==NULL){
                        //alone in the network
                        fillContact(node_info->ext, node_info->id, node_info->ip, node_info->port);
                    }else{
                        promoteEXT(node_info);
                        //send extern message to new extern since it is now an anchor node
                        extern_send(node_info, node_info->ext->fd);
                    }
                }else{
                    int new_fd = connecttonode(node_info->bck->ip, node_info->bck->port);
                    if(new_fd==-1){
                        fprintf(stderr, "failed to connect to back up.\n");
                        return -2;
                    }        
                    new_send(new_fd, node_info->id, node_info->ip, node_info->port);
                    char buff[128];
                    int n = readwithtimeout(new_fd, 2, buff, 128);
                    if(n == -1){
                        perror("error on read");
                        fprintf(stderr, "connecting node did not respond.\n");
                        return -2;
                    }else if(n == 0){
                        fprintf(stderr, "connecting node did not respond.\n");
                        return -2;
                    }else{
                        char **args = (char**)malloc(6*sizeof(char*));
                        n = messagecheck(buff, args);
                        if(n == 1){
                            //set node info extern
                            node_info->ext = createContact();
                            node_info->ext->fd = new_fd;
                            fillContact(node_info->ext,node_info->bck->id, node_info->bck->ip, node_info->bck->port);
                            n = extern_rcv(node_info, node_info->bck->id, args[0], args[1], args[2]);
                            if(n==-1){
                                fprintf(stderr, "connecting node did not respond acoordingly.\n");
                                free(args);
                                return -2;
                            }
                            FD_SET(new_fd, rfds);
                        }else{
                            fprintf(stderr, "connecting node did not respond acoordingly.\n");
                            free(args);
                            return -2;
                        }
                        free(args);
                    }
                }
                //send extern message to all intern nodes
                for(Contact aux = node_info->intr; aux != NULL; aux = aux->next){
                    extern_send(node_info, aux->fd);
                }
            }else{
                //if intern contact, remove from intern list
                FD_CLR(contact->fd,rfds);
                node_info->intr = removeContact(node_info->intr, contact);
            }
        }else{
            //handle message
            strcat(contact->stored, buffer);
            while(process_stream(contact->stored, buffer) == 0){
                char **args = (char**)malloc(6*sizeof(char*));
                int msg = messagecheck(buffer, args);
                switch(msg){
                    case 0:
                        //new
                        if(new_rcv(node_info, contact, args[0], args[1], args[2])!=0){
                            fprintf(stderr, "error recieving new msg.\n");
                            return -1;
                        }
                    break;
                    case 1:
                        //extern
                        if(extern_rcv(node_info, contact->id, args[0], args[1], args[2])!=0){
                            fprintf(stderr, "error recieving extern msg.\n");
                            return -1;
                        }
                    break;
                    case 2:
                        //withdraw
                        if(withdraw_rcv(node_info, contact, args[0])!=0){
                            fprintf(stderr, "error recieving withdraw msg.\n");
                            return -1;
                        }
                    break;
                    case 3:
                        //query
                        if(query_rcv(node_info, contact, args[0], args[1], args[2])!=0){
                            fprintf(stderr, "error in query msg receive protocol.\n");
                            return -1;
                        }
                    break;
                    case 4:
                        //content
                        content_rcv(node_info, contact, args[0], args[1], args[2]);
                    break;
                    case 5:
                        //nocontent
                        nocontent_rcv(node_info, contact, args[0], args[1], args[2]);
                    break;
                    case -1:
                        //error
                        fprintf(stderr, "error: message does not correspond to any node protocol.\n");
                        return -1;
                    break;
                    default:
                        fprintf(stderr, "error: message does not correspond to any node protocol.\n");
                        return -1;
                    break;

                }
                free(args);
            }
        }
    }
    return 0;
}

int checknewconnection(int fd_tcp, struct node_info* node_info, fd_set* aux_rfds, fd_set* rfds){
    if(FD_ISSET(fd_tcp, aux_rfds)){
        //create new fd to deal with message
        int new_fd;
        struct sockaddr* addr;
        socklen_t addr_size = sizeof(addr);
        if((new_fd = accept(fd_tcp, (struct sockaddr*) &addr, &addr_size)) == -1){
            fprintf(stderr, "error accepting new connection.\n");
            return -1;
        }else{
            printf("accepted a connection.\n");
            //create new contact with default data (to be filled later)
            node_info->intr = addContact(node_info->intr, "-1", new_fd);
            //wait for new
            char buff[128];
            int n = readwithtimeout(new_fd, 2, buff, 128);
            if(n == -1){
                perror("read error");
                fprintf(stderr, "connecting node did not respond.\n");
                node_info->intr = removeContact(node_info->intr, getContact(node_info->intr,"-1"));
                return -1;
            }else if(n == 0){
                fprintf(stderr, "connecting node did not respond.\n");
                node_info->intr = removeContact(node_info->intr, getContact(node_info->intr,"-1"));
                return -1;
            }else{
                char **args = (char**)malloc(6*sizeof(char*));
                n = messagecheck(buff, args);
                if(n == 0){
                    n = new_rcv(node_info, getContact(node_info->intr,"-1"), args[0], args[1], args[2]);
                    if(n==-1){
                        fprintf(stderr, "connecting node did not respond acoordingly.\n");
                        node_info->intr = removeContact(node_info->intr, getContact(node_info->intr,"-1"));
                        free(args);
                        return -1;
                    }
                    //set node info intern
                    FD_SET(new_fd, rfds);
                    
                }else{
                    fprintf(stderr, "connecting node did not respond acoordingly.\n");
                    node_info->intr = removeContact(node_info->intr, getContact(node_info->intr,"-1"));
                    free(args);
                    return -1;
                }
                free(args);
            }
            //add file descriptor to select checks
            FD_SET(new_fd, rfds);
        }
    }
    return 0;
}

int process_stream(char stored[], char msg[]){
    char buffer[2*(MAX_MSG+1)];
    char *aux;

    memcpy(buffer, stored, 2*(MAX_MSG+1));

    if(strlen(stored) != 0){
        aux = strtok(buffer, "\n");
        if(aux == NULL){
            return -1;
        }
    }
    else{
        return -1;
    }

    if(strlen(msg) > MAX_MSG-1){
        //discard
        memset(stored, '\0', 2*(MAX_MSG+1));
        return -1;
    }
    else {
        strcpy(msg, aux);
        if(strlen(msg) < strlen(stored)){  //found a complete message with reasonable size
            //reattach \n symbol to msg (strtok removed it)
            strcat(msg, "\n");
            //shift left in stored
            memmove(stored, &stored[strlen(msg)], 2*(MAX_MSG+1)-strlen(msg));

            return 0;
        }
        else if(strlen(msg) == strlen(stored)){ //incomplete message
            //wait for complete
        }
        return -1;
    }
    return 0;
}