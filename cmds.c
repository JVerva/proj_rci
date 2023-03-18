#include "cmds.h"

const char* CMDS[] = {"join", "djoin", "create", "delete", "get", "show", "topology", "names", "routing", "leave", "exit"};

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
        if(i>6){
            fprintf(stderr,"too many arguments.\n");
            return -1;
        }
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
int join(int fd_udp, int fd_tcp, char net[], char id[], char ip[], char tcp[], struct addrinfo serverinfo){
    //verify arguments
    if(verifynet(net)!=0){
        fprintf(stderr, "net id is invaid.\n");
        return -1;
    }
    if(verifyid(id)!=0){
        fprintf(stderr, "node id is invaid.\n");
        return -1;
    }
    //ask for network info
    char buff[256];
    char* node_list;
    char cmd[40] = {"NODES "};
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
    //check if id already exists in the network
    int changed = 1;
    char* og_id = strdup(id);
    char* tmpbuff = strdup(buff);
    node_list = strdup(buff);
    //if it does get the next free id, and add it with that 
    while(checkfornode(id, tmpbuff)!=0){
        int iid = atoi(id);
        iid++;
        sprintf(id, "%.2d", iid);
        tmpbuff = strdup(buff);
        changed = 0;
    }
    if(changed == 0){
        printf("%s was already in the network, using id %s instead.\n", og_id, id);
    }
    //create node in the network
    memset(cmd, 0, sizeof(cmd));
    memset(buff, 0, sizeof(buff));
    sprintf(cmd, "REG %s %s %s %s", net, id, ip, tcp);
    n = sendto(fd_udp, cmd , 40 ,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
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
    if(strcmp(buff, "OKREG")==0){
        printf("node inserted.\n");
    }
    //ask which node you want to connect
    char node[10];
    char *t_ip, *t_port;
    printf("which node do you wish to connect to?\n %s\n", node_list);
    int valid = 1;
    do{
        scanf("%s", node);
        if(verifyid(node)!=0){
            fprintf(stderr,"wrong format for node id.\n");
        }else if(checkfornode(node, node_list)!=1){
            fprintf(stderr, "node does not exist in the network.\n");
        }else{
            getnodeinfo(node, node_list, &t_ip, &t_port);
            valid = 0;
        }
    }while(valid != 0);

    //try to connect to node
    struct addrinfo hints, *res;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    

    n=getaddrinfo(t_ip, t_port ,&hints, &res);
    if(n!=0){
        fprintf(stderr,"error getting node address.\n");
        return -1;
    }

    n=connect(fd_tcp,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        fprintf(stderr,"error connecting to node.\n");
        return -1;
    }

    printf("connected to node sucessfully.\n");

    //send connection message
    char msg[30];
    sscanf(msg, "NEW %s %s %s", id, ip, tcp);
    write(fd_tcp, msg ,30);

    return 0;
}

//joins node to network.
int djoin(int fd_udp,int fd_tcp, char net[], char id[], char bootid[], char ip[], char tcp[], struct addrinfo serverinfo){
    //verify arguments
    if(verifynet(net)!=0){
        fprintf(stderr, "net id is invaid.\n");
        return -1;
    }
    if(verifyid(id)!=0){
        fprintf(stderr, "node id is invaid.\n");
        return -1;
    }
    if(verifyid(bootid)!=0){
        fprintf(stderr, "boot node id is invaid.\n");
        return -1;
    }
    char buff[256];
    char cmd[40];
    //create node in the network
    memset(cmd, 0, sizeof(cmd));
    memset(buff, 0, sizeof(buff));
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
    if(strcmp(buff, "OKREG")==0){
        printf("node inserted.\n");
    }


    //if boot id and id are not same, connect to boot node
    if(strcmp(bootid,id)!=0){
        //get node list
        char node_list[128];
        //ask for network info
        char cmd[10] = {"NODES "};
        strcat(cmd, net);
        int n = sendto(fd_udp, cmd ,9,0,serverinfo.ai_addr, serverinfo.ai_addrlen);
        if(n == -1){
            perror("sendto error");
            return -1;
        }
        n = recvfrom(fd_udp, node_list,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
        if(n == -1){
            perror("rcvfrom error");
            return -1;
        }

        //try to connect
        char *t_ip, *t_port;
        if(getnodeinfo(bootid, node_list, &t_ip, &t_port)!=0){
            fprintf(stderr,"error get node info.\n");
            return -1;
        }

        struct addrinfo hints, *res;
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        

        n=getaddrinfo(t_ip, t_port ,&hints, &res);
        if(n!=0){
            fprintf(stderr,"error getting node address.\n");
            return -1;
        }

        n=connect(fd_tcp,res->ai_addr,res->ai_addrlen);
        if(n==-1){
            fprintf(stderr,"error connecting to node.\n");
            return -1;
        }

        printf("connected to node sucessfully.\n");

        //send connection message
        char msg[30];
        sscanf(msg, "NEW %s %s %s", id, ip, tcp);
        write(fd_tcp, msg ,30);
    }
    return 0;
}

//unregisters node from network
int leave(int udp, char net[], int id, struct addrinfo serverinfo){
    //send leave command
    char buff[256];
    char cmd[13];
    char ok_unreg[] = "OKUNREG";
    sprintf(cmd, "UNREG %s %.2d", net, id);
    printf("%s\n",cmd);

    int n = sendto(udp, cmd, 12, 0, serverinfo.ai_addr, serverinfo.ai_addrlen);
    if(n == -1){
        fprintf(stderr, "sendto error.\n");
        return -1;
    }

    //receive server response
    n = recvfrom(udp, buff,256,0,serverinfo.ai_addr,&serverinfo.ai_addrlen);
    if(n == -1){
        fprintf(stderr, "recvfrom error.\n");
        return -1;
    }
    if (strcmp(ok_unreg, buff)!=0){
        fprintf(stderr, "leave command not successful\n");
        return -1;
    }
    printf("node left.\n");
    return 0;
}

