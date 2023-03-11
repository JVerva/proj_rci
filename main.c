#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//a

int inittcpsocket(char[], char[]);
int initudpsocket(char[], char[], struct addrinfo*);
int commandcheck(char[], char*[]);
int join(int, char[] , char[], struct addrinfo);
int checkfornode(char[], char[]);

const char* CMDS[] = {"join", "djoin", "create", "delete", "get", "show", "topology", "names", "routing", "leave", "exit"};
char DEFAULT_IP[] = {"193.136.138.142"};
char DEFAULT_PORT[] = {"59000"};

int main(int argc, char* argv[]){

    //get arguments
    if(argc != 5 && argc != 3){
        fprintf(stderr, "wrong number of arguments: %d.\n", argc);
        exit(1);
    }if (argc == 5){
        char *regIp = argv[3];
        char *regPort = argv[4];
    }else if(argc == 3){
        char *regIp = DEFAULT_IP;
        char *regPort = DEFAULT_PORT;
    }

    char *ip = argv[1];
    char *port = argv[2];

    struct addrinfo node_server;

    //init tcp socket
    int fd_tcp = inittcpsocket(ip, port);
    //init udp sockt to comunicate with network server
    int fd_udp = initudpsocket(ip, port, &node_server);


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
        FD_SET(fd_tcp, &rfds);

        counter = select(fd_tcp+1, &rfds, NULL, NULL, NULL);

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
                char **args = (char**)malloc(5*sizeof(char**));
                int cmd = commandcheck(buffer, args);
                switch(cmd){
                    case -1:
                        break;
                    case 0:
                        join(fd_udp, args[0], args[1], node_server);
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
            if(FD_ISSET(fd_tcp, &rfds)){
                FD_CLR(fd_tcp,&rfds);
                n = read(0,buffer,128);
                printf("socket: %s\n", buffer);
            }
            strcpy(buffer,"\0");
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
int initudpsocket(char ip[], char port[], struct addrinfo *res){
    int status;
    struct addrinfo hints;
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
int join(int udp, char net[], char id[], struct addrinfo serverinfo){
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
    char cmd[10] = {"NODES \0"};
    int n = sendto(udp, strcat(cmd, net),9,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        fprintf(stderr, "sendto error.\n");
        return -1;
    }
    n = recvfrom(udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        fprintf(stderr, "recvfrom error.\n");
        return -1;
    }
    //check if id already exists in the network
    int changed = 1;
    char* og_id = id;
    while(checkfornode(id, buff)!=0){
        int iid = atoi(id);
        iid++;
        sprintf(id, "%d", iid);
        if(iid<10){
            id = strcat("0",id);
        }
        changed = 0;
    }
    if(changed == 0){
        printf("%s was already in the network, used id: %s instead.", og_id, id);
    }
}

//check is node already exists in network, node list is the list of nodes returned by network
int checkfornode(char node_id[], char node_list[]){
    //divide buffer into tokens
    //extract the first token
    char* token = strtok(node_list, " \n");
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(strcmp(node_id,token)==0){
            //node already exists
            return 1;
        }
    }
    return 0;
}

