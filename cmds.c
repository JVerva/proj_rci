#include "cmds.h"

const char* CMDS[] = {"join", "djoin", "create", "delete", "get", "show", "topology", "names", "routing", "leave", "exit", "st", "sn", "sr", "cr"};

//checks if commands are valid, returns comand index if they are, returns -1 and prints error msg if they aren't
//args gets filled with arguments for command
int commandcheck(char buffer[], char** args){
    int index = -1;
    int n = -1;
    char cmd[20];
    char* token = NULL;
    //divide buffer into tokens
    //extract the first token
    token = strtok(buffer, " \n");
    if(token!=NULL){
        strcpy(cmd,token);
    }
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(i>5){
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
        }else{
            index = 4;
        }
    }else if(strcmp(cmd,CMDS[5])==0){
        if(n!=1){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[5]);
            return -1;
        }else if(strcmp(args[0],CMDS[6])==0){
            index = 6;
            args[0] = args[1];
            args[1] = NULL;
        }else if(strcmp(args[0],CMDS[7])==0){
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
    }else if(strcmp(cmd,CMDS[11])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[10]);
            index = -1;
        }else{
            index = 6;
        }
    }else if(strcmp(cmd,CMDS[12])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[10]);
            index = -1;
        }else{
            index = 7;
        }
    }else if(strcmp(cmd,CMDS[13])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[10]);
            index = -1;
        }else{
            index = 8;
        }
    }else if(strcmp(cmd,CMDS[14])==0){
        if(n!=0){
            fprintf(stderr, "%s error: wrong number of arguments.\n", CMDS[10]);
            index = -1;
        }else{
            index = 11;
        }
    }else{
        fprintf(stderr, "error: %s is not a valid command.\n", cmd);
        return -1;
    }
    return index;
}

//joins node to network. if node id is already in the networks, use a new free one.
int join(int fd_udp, int fd_tcp, struct node_info* nodeinfo, char net[], char id[], char ip[], char tcp[], struct addrinfo serverinfo, fd_set* rfds){
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
    int new_fd;
    char node_list[256];
    memset(node_list,0,256);
    if(getnodelist(fd_udp, serverinfo, net, node_list)!=0){
        return -1;
    }
    //check if id already exists in the network
    int changed = 1;
    char og_id[3] = {"\0"};
    strcpy(og_id, id);
    //if it does, get the next free id, and add it with that 
    while(checkfornode(id, node_list)!=0){
        int iid = atoi(id);
        sprintf(id, "%.2d", ++iid);
        changed = 0;
    }
    if(changed == 0){
        printf("%s was already in the network, using id %s instead.\n", og_id, id);
    }
    //create node in the network
    if(registernode(fd_udp, serverinfo, net, id, ip, tcp)!=0){
        return -1;
    }
    //set node info id
    strcpy(nodeinfo->id,id);
    strcpy(nodeinfo->ip, ip);
    strcpy(nodeinfo->port, tcp);

    printf("node inserted.\n");
    //default node info
    nodeinfo->bck = createContact();
    strcpy(nodeinfo->bck->id,id);
    //check if there are any other nodes in the network
    if(isnetworkempty(node_list)==0){
        //set node info extern
        nodeinfo->ext = createContact(); 
        strcpy(nodeinfo->ext->id,id);
        strcpy(nodeinfo->ext->ip,ip);
        strcpy(nodeinfo->ext->port,tcp);
        printf("first node in the network.\n");
    }else{
        //ask which node you want to connect
        char node[10];
        char t_ip[20], t_port[6];
        memset(t_ip,0,20);
        memset(t_port,0,6);
        printf("which node do you wish to connect to?\n %s", node_list);
        int valid = 1;
        do{
            scanf("%s", node);
            if(verifyid(node)!=0){
                fprintf(stderr,"wrong format for node id.\n");
            }else if(checkfornode(node, node_list)!=1){
                fprintf(stderr, "node does not exist in the network.\n");
            }else{
                getnodeinfo(node, node_list, t_ip, t_port);
                valid = 0;
            }
        }while(valid != 0);
 
        new_fd = connecttonode(t_ip, t_port);
        if(new_fd==-1){
            //unregister node
            unregisternode(fd_udp, serverinfo, net, id);
            close(fd_tcp);
            return -1;
        }
        //set node info extern
        FD_SET(new_fd, rfds);
        nodeinfo->ext = createContact();
        nodeinfo->ext->fd = new_fd;
        fillContact(nodeinfo->ext,node, t_ip,t_port);
        //send connection message
        new_send(new_fd, id, ip, tcp);
    }
    return 0;
}

//joins node to network.
int djoin(int fd_tcp, struct node_info* node_info, char net[], char id[], char bootid[], char ip[], char tcp[], struct addrinfo serverinfo){
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
    //set node info id
    strcpy(node_info->id,id);
    int new_fd;
    //if boot id and id are not same, connect to boot node
    if(strcmp(bootid,id)!=0){
        new_fd = connecttonode(ip, tcp);
        if(new_fd == -1){
            //error connection to boot node
            close(fd_tcp);
            return -1;
        }
        //set extern node
        node_info->ext = createContact();
        fillContact(node_info->ext, bootid, ip, tcp);
        //send connection message
        char msg[30];
        sscanf(msg, "NEW %s %s %s", id, ip, tcp);
        write(new_fd, msg ,30);
    }
    return 0;
}

//unregisters node from network
int leave(int udp, int tcp, char net[], char id[],struct node_info** node_info,  struct addrinfo serverinfo, fd_set* rfds){
    //unregisters node in network
    if(unregisternode(udp, serverinfo, net, id)!=0){
        return -1;
    }
    //clear sockets from select
    FD_CLR((*node_info)->ext->fd, rfds);
    FD_CLR(tcp, rfds);
    for(Contact aux = (*node_info)->intr; aux != NULL; aux = aux->next){
        FD_CLR(aux->fd, rfds);
    }
    //close all opened sockets exept udp
    //reset node info
    closeNode_info(*node_info);
    *node_info = initNode_info();
    close(tcp);

    return 0;
}


int show_topology(struct node_info* node){
    Contact aux;

    printf(" %s%s%-10s%s | %s%-7s%s | %s%-17s%s | %s%s%s%s\n", BOLD, YEL, "NEIGHBOR", WHI, YEL, "ID", WHI, YEL, "IP", WHI, YEL, "PORT", NORM, NBOLD);
    printf("---------------------------------------------------------\n");
    
    if(strcmp(node->ext->id, node->id)==0){
        printf("*%-10s | %-7s | %-17s | %s\n", "EXT", node->id, "self", "self");
    }else{
        printf("*%-10s | %-7s | %-17s | %s\n", "EXT", node->ext->id, node->ext->ip, node->ext->port);
    }
    printf("---------------------------------------------------------\n");
    
    if(strcmp(node->bck->id, node->id)==0){
        printf("*%-10s | %-7s | %-17s | %s\n", "BCK", node->id, "self", "self");
    }else{
        printf("*%-10s | %-7s | %-17s | %s\n", "BCK", node->bck->id, node->bck->ip, node->bck->port);
    }
    printf("---------------------------------------------------------\n");

    if(node->intr == NULL){
        printf("*%-10s | %-7s | %-17s | %s\n", "INTR", "--", "--", "--");
    }else{
        printf("*%-10s | %-7s | %-17s | %s\n", "INTR", node->intr->id, node->intr->ip, node->intr->port);
        aux = node->intr->next;
        while(aux != NULL){
            if(strcmp(aux->id, "-1") != 0){
                printf(" %-10s | %-7s | %-17s | %s\n", " ", aux->id, aux->ip, aux->port);
            }
            aux = aux->next;     
        }
    }
    printf("---------------------------------------------------------\n");
    return 0;
}

int get(struct node_info* nodeinfo, char dest[], char name[]){
    Contact route_dest, aux;
    //check input error||||||||||||||||||||||||
    if(strcmp(nodeinfo->id, dest)==0){
        printf("cannot get name from self.\n");
        return 0;
    }
    //check if dest is in routing table
    if((route_dest = checkRoute(nodeinfo->rout_table, dest)) == NULL){//if not found  
        //send query to EXT
        if(nodeinfo->ext->fd != -1){
            query_send(nodeinfo->ext->fd, dest, nodeinfo->id, name);
        }else if(nodeinfo->intr == NULL){
            printf("no neighbors to get name from.\n");
        }
        //send query to every internal neighbor
        aux = nodeinfo->intr;
        while(aux != NULL){
            query_send(aux->fd, dest, nodeinfo->id, name);
        }
    }else{
        //send QUERY through route
        query_send(route_dest->fd, dest, nodeinfo->id, name);
    }
    return 0;
}