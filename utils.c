#include "utils.h"

//cheks if net is valid
int verifynet(char* net){
    if(strlen(net) != 3){
        return 1;
    }else{
        for(int i = 0; i < 3; i++){
            if(!(net[i]<57 && net[i]>47)){
                return 1;
            }
        }
    }
    return 0;
}

//checks if id id valid
int verifyid(char* id){
    if(strlen(id) != 2){
        return 1;
    }else{
        for(int i = 0; i < 2; i++){
            if(!(id[i]<57 && id[i]>47)){
                return 1;
            }
        }
    }
    return 0;
}

//check if node already exists in network, node list is the list of nodes returned by network, returns 1 if it already exists
int checkfornode(char node_id[], char node_list[]){
    char *temp = strdup(node_list);
    //divide buffer into tokens
    //extract the first token
    char* token = strtok(temp, " \n");
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(token!=NULL){
            if(strcmp(node_id,token)==0){
                //node already exists
                free(temp);
                return 1;
            }
        }
    }
    free(temp);
    return 0;
}

//check if there are existing nodes in the network
int isnetworkempty(char node_list[]){
    char *temp = strdup(node_list);
    //divide buffer into tokens
    //extract the first token
    char* token = strtok(temp, "\n");
    token = strtok(NULL, "\n");

    if(token == NULL){
        free(temp);
        return 0;
    }else{
        free(temp);
        return 1;
    }
}

int getnodeinfo(char node_id[], char node_list[], char ** ip, char ** port){
    //divide buffer into tokens
    //extract the first token
    char* temp = strdup(node_list);
    char* token = strtok(temp, " \n");
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(token!=NULL){
            if(strcmp(node_id,token)==0){
                //node exists
                token = strtok(NULL, " ");
                *ip = strdup(token);
                token = strtok(NULL, " ");
                *port = strdup(token);
                free(temp);
                return 0;
            }
        }
    }
    free(temp);
    return -1;
}

//returns a tcp socket connected to input adress
int connecttonode(char *t_ip, char *t_port){
    struct addrinfo hints, *res;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int n=getaddrinfo(t_ip, t_port ,&hints, &res);
    if(n!=0){
        fprintf(stderr,"error getting node address.\n");
        perror("error");
        return -1;
    }
    
    int fd_tcp = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    n=connect(fd_tcp,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        fprintf(stderr,"error connecting to node.\n");
        return -1;
    }

    printf("connected to node sucessfully.\n");

    return fd_tcp;
}

//gets nodelist from network
int getnodelist(int fd_udp,struct addrinfo serverinfo, char* net, char** node_list){
    char buff[256];
    char cmd[10] = {"NODES "};
    strcat(cmd, net);
    int n = sendto(fd_udp, cmd ,9,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        perror("sendto error");
        return -1;
    }
    n = recvfrom(fd_udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        perror("rcvfrom error");
        return -1;
    }
    *node_list = strdup(buff);
    return 0;
}

//creates node in network
int createnode(int fd_udp,struct addrinfo serverinfo, char* net, char* id, char* ip, char* tcp){
    char cmd[30];
    char buff[256];
    memset(buff,0,256);
    memset(cmd,0,30);
    sprintf(cmd, "REG %s %s %s %s", net, id, ip, tcp);
    int n = sendto(fd_udp, cmd , 40 ,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        perror("sendto error");
        return -1;
    }
    //confirm node insertion
    n = recvfrom(fd_udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        perror("rcvfrom error");
        return -1;
    }
    if(strcmp(buff, "OKREG")!=0){
        return -1;
    }
    return 0;
}
