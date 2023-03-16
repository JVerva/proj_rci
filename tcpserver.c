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

int inittcpsocket(char[], char[]);
int initudpsocket(char[], char[], struct addrinfo**);
int commandcheck(char[], char*[]);
int join(int, char[] , char[], char[], char[], struct addrinfo);
int djoin(int, char[] , char[], char[], char[],char[], struct addrinfo);
int checkfornode(char[], char[]);



typedef struct contact{
    char* id;
    int fd;
    char *ip;
    char *port;
    Contact *next;
    Contact *prev;
}Contact;


struct node{
    char* id;
    Contact* ext;
    Contact* bck;
    int intr[];//||||||||||||||||||||||||||||||||||||||
};


const char* CMDS[] = {"join", "djoin", "create", "delete", "get", "show", "topology", "names", "routing", "leave", "exit"};
char DEFAULT_IP[] = {"193.136.138.142"};
char DEFAULT_PORT[] = {"59000"};

int main(int argc, char* argv[]){
    char *reg_ip = (char*)malloc(sizeof(char*));
    char *reg_port = (char*)malloc(sizeof(char*));
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
    int fd_tcp = inittcpsocket(ip, port);
    struct sockaddr_in addr;
    socklen_t addrlen;
    int newfd;
    //init udp sockt to comunicate with network server
    int fd_udp = initudpsocket(reg_ip, reg_port, &node_server);


    //file descriptor sets
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

    //contact list
    Contact* contact_head = NULL;    
    Contact* contact_tail = NULL;
    Contact* aux_contact;


    //program loop
    while(1){

        aux_rfds = rfds;

        counter = select(fd_tcp+1, &raux_fds, NULL, NULL, NULL);

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
                char **args = (char**)malloc(6*sizeof(char**));
                int cmd = commandcheck(buffer, args);
                switch(cmd){
                    case -1:
                        //error on commands (skip)
                        break;
                    case 0:
                        //join
                        join(fd_udp, args[0], args[1], ip, port, *node_server);
                        break;
                    case 1:
                        //djoin
                        djoin(fd_udp, args[0], args[1], args[2], args[3], args[4], *node_server);
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
                addrlen = sizeof(addr);
                //accept new connection
                if((newfd = accept(fd_tcp, (struct sockaddr*) &addr, &addrlen)) == -1){
                    fprintf(stderr, "Error on accept new connection.\n");
                }
                //add new conection to Select
                FD_SET(newfd, &rfds);
                //create new contact information
                Contact *new_contact = (Contact *)calloc(1, sizeof(Contact));
                new_contact->fd = newfd;
                strdup(new_contact->id, "-1");
                if(contact_head == NULL){
                    contact_head = new_contact;
                    contact_tail = new_contact;
                }
                else{
                    contact_tail->next = new_contact;
                    new_contact->prev = contact_tail;
                    contact_tail = new_contact;
                }
            }
            //any neighbor ready
            aux_contact = contact_head;
            while(aux_contact != NULL){
                if(FD_ISSET(aux_contact->fd, &aux_rfds)){


                    //do stuff

                }
                aux_contact = aux_contact->next;
            }
            memset(buffer,0,sizeof(buffer));
        }

    }
    return 0;
}

//create a tcp socket, bind to port and listen for incoming connections
int inittcpsocket(char ip[], char port[]){
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
int initudpsocket(char ip[], char port[], struct addrinfo **res){
    int status;
    struct addrinfo hints;
    int sockfd;

    //fill hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //get address info
    if(status = getaddrinfo(ip, port, &hints, res) != 0){
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //create socket
    sockfd = socket((*res)->ai_family, (*res)->ai_socktype, (*res)->ai_protocol);

    return sockfd;
}

//checks if commands are valid, returns comand index if they are, returns -1 and prints error msg if they aren't
//args gets filled with arguments for command
int commandcheck(char buffer[], char** args){
    int index = 0;
    int n = -1;
    char cmd[10];

    //divide buffer into tokens
    //extract the first token
    char* token = strtok(buffer, " \n");
    if(token!=NULL){
        strcpy(cmd,token);
    }
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        args[i] = token;
        n++;
    }

    //do all checks
    if(strcmp(cmd,CMDS[0])==0){
        if(n!=2){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[0]);
            index = -1;
        }else{
            index = 0;
        }
    }else if(strcmp(cmd,CMDS[1])==0){
        if(n!=5){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[1]);
            index = -1;
        }else{
            index = 1;
        }
    }else if(strcmp(cmd,CMDS[2])==0){
        if(n!=1){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[2]);
            index = -1;
        }else{
            index = 2;
        }
    }else if(strcmp(cmd,CMDS[3])==0){
        if(n!=1){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[3]);
            index = -1;
        }else{
            index = 3;
        }
    }else if(strcmp(cmd,CMDS[4])==0){
        if(n!=2){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[4]);
            index = 4;
        }
    }else if(strcmp(cmd,CMDS[5])==0){
        if(n!=2){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[5]);
            return -1;
        }else if(strcmp(args[0],CMDS[6])==0){
            index = 6;
            args[0] = args[1];
            args[1] = NULL;
        }else if(strcmp(args[0],CMDS[6])==0){
            index = 7;
            args[0] = args[1];
            args[1] = NULL;
        }else if(strcmp(args[0],CMDS[8])==0){
            index = 8;
            args[0] = args[1];
            args[1] = NULL;
        }
    }else if(strcmp(cmd,CMDS[9])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[9]);
            index = -1;
        }else{
            index = 9;
        }
    }else if(strcmp(cmd,CMDS[10])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[10]);
            index = -1;
        }else{
            index = 10;
        }
    }else{
        fprintf(stderr, "error: %s is not a valid command.\n", cmd);
        return -1;
    }
    return index;
}

//joins node to network. if node id is already in the networks, use a new free one.
int join(int udp, char net[], char id[], char ip[], char tcp[], struct addrinfo serverinfo){
    //verify arguments
    if(strlen(net) != 3){
        fprintf(stderr, "net id has wrong size.\n");
        return -1;
    }else{
        for(int i = 0; i < 3; i++){
            if(!(net[i]<57 && net[i]>47)){
                fprintf(stderr, "net id is invaid.\n");
                return -1;
            }
        }
    }
    if(strlen(id) != 2){
        fprintf(stderr, "node id has wrong size.\n");
        return -1;
    }else{
        for(int i = 0; i < 2; i++){
            if(!(id[i]<57 && id[i]>47)){
                fprintf(stderr, "node id is invaid.\n");
                return -1;
            }
        }
    }

    //ask for network info
    char buff[256];
    char cmd[40] = {"NODES \0"};
    strcat(cmd, net);
    int n = sendto(udp, cmd ,9,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        perror("sendto error");
        return -1;
    }
    n = recvfrom(udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        perror("rcvfrom error");
        return -1;
    }
    //check if id already exists in the network
    int changed = 1;
    char* og_id = strdup(id);
    char* tmpbuff = strdup(buff);
    //if it does get the next free id, and add it with that 
    while(checkfornode(id, tmpbuff)!=0){
        int iid = atoi(id);
        iid++;
        sprintf(id, "%.2d", iid);
        tmpbuff = strdup(buff);
        changed = 0;
    }
    if(changed == 0){
        printf("%s was already in the network, used id %s instead.\n", og_id, id);
    }
    //create node in the network
    memset(cmd, 0, sizeof(cmd));
    memset(buff, 0, sizeof(buff));
    sprintf(cmd, "REG %s %s %s %s", net, id, ip, tcp);
    n = sendto(udp, cmd , 40 ,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        perror("sendto error");
        return -1;
    }
    //confirm node insertion
    n = recvfrom(udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        perror("rcvfrom error");
        return -1;
    }
    if(strcmp(buff, "OKREG")==0){
        printf("node inserted.\n");
    }
}

//joins node to network.
int djoin(int udp, char net[], char id[], char bootid[], char ip[], char tcp[], struct addrinfo serverinfo){
    //verify arguments
    if(strlen(net) != 3){
        fprintf(stderr, "net id has wrong size.\n");
        return -1;
    }else{
        for(int i = 0; i < 3; i++){
            if(!(net[i]<57 && net[i]>47)){
                fprintf(stderr, "net id is invaid.\n");
                return -1;
            }
        }
    }
    if(strlen(id) != 2){
        fprintf(stderr, "node id has wrong size.\n");
        return -1;
    }else{
        for(int i = 0; i < 2; i++){
            if(!(id[i]<57 && id[i]>47)){
                fprintf(stderr, "node id is invaid.\n");
                return -1;
            }
        }
    }
    char buff[256];
    char cmd[40];
    //if boot id and id are the same, just add a node to network
    if(strcmp(bootid,id)==0){
        //create node in the network
        memset(cmd, 0, sizeof(cmd));
        memset(buff, 0, sizeof(buff));
        sprintf(cmd, "REG %s %s %s %s", net, id, ip, tcp);
        int n = sendto(udp, cmd , 40 ,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
        if(n == -1){
            perror("sendto error");
            return -1;
        }
        //confirm node insertion
        n = recvfrom(udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
        if(n == -1){
            perror("rcvfrom error");
            return -1;
        }
        if(strcmp(buff, "OKREG")==0){
            printf("node inserted.\n");
        }
    }
}


//check if node already exists in network, node list is the list of nodes returned by network, returns 1 if it already exists
int checkfornode(char node_id[], char node_list[]){
    //divide buffer into tokens
    //extract the first token
    char* token = strtok(node_list, " \n");
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(token!=NULL){
            if(strcmp(node_id,token)==0){
                //node already exists
                return 1;
            }
        }
    }
    return 0;
}

