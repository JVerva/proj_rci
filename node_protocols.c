#include "node_protocols.h"

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

struct node_info* initNode_info(){
    struct node_info *temp = (struct node_info*)malloc(sizeof(struct node_info));
    strcpy(temp->id, "-1");
    strcpy(temp->ip, "-1");
    strcpy(temp->port, "-1");
    temp->intr = NULL;
    temp->ext = NULL;//must be closed|||||||||||||||
    temp->bck = NULL;
    temp->rout_table = NULL;
    temp->names = NULL;
    return temp;
}

void closeNode_info(struct node_info *node){
    removeContact(node->bck,node->bck);
    removeContact(node->ext,node->ext);
    closeContacts(node->intr);
    closeNames(node->names);
    closeRoutingTable(node->rout_table);
    free(node);
}

int messagecheck(char buffer[], char** args){
    int index = 0;
    int n = -1;
    char msg[10];
    memset(msg,0,10);

    //divide buffer into tokens
    //extract the first token
    char* token = strtok(buffer, " \n");
    if(token != NULL){
        strcpy(msg, token);
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
    if(strcmp(msg, MSGS[0])==0){
        if(n!=3){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 0;
        }
    }else if(strcmp(msg, MSGS[1])==0){
        if(n!=3){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[1]);
            index = -1;
        }else{
            index = 1;
        }
    }else if(strcmp(msg, MSGS[2])==0){
        if(n!=1){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[2]);
            index = -1;
        }else{
            index = 2;
        }
    }else if(strcmp(msg, MSGS[3])==0){
        if(n!=3){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[3]);
            index = -1;
        }else{
            index = 3;
        }
    }else if(strcmp(msg, MSGS[4])==0){
        if(n!=3){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[4]);
            index = -1;
        }else{
            index = 4;
        }
    }else if(strcmp(msg, MSGS[5])==0){
        if(n!=3){
            fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[5]);
            index = -1;
        }else{
            index = 5;
        }
    }else{
        fprintf(stderr, "error: %s is not a valid message.\n", msg);
        return -1;
    }
    return index;
}

int extern_rcv(struct node_info *node, char id_sender[],char id_rcv[], char ip[], char port[]){
    //maybe check for input error|||||||||||||||||||||||||||
    if(verifyid(id_rcv)!=0){
        printf("EXTERN message with invalid id\n");
        return -1;
    }

    //check if message is coming from ext neighbor, else do nothing, a bad behaving node may have sent the message instead
    if(strcmp(node->ext->id, id_sender) != 0){
        return -1;
    }else{
        //update backup neighbor contact information
        fillContact(node->bck, id_rcv, ip, port);
        node->bck->fd = -1; //|||||||||||

        //UPDATE ROUTING TABLE?|||||||||||||
    }
    return 0;
}

int new_rcv(struct node_info* nodeinfo, Contact sender, char id_rcv[], char ip[], char port[]){
    //INPUT ERROR CHECK||||||||||||||||||
    //unknown contact
    if(strcmp(sender->id, "-1") == 0){
        fillContact(sender, id_rcv, ip, port);
    }
    //if node is alone, incoming node becomes its ext
    if(strcmp(nodeinfo->id, nodeinfo->ext->id) == 0){
        promoteEXT(nodeinfo);
    }
    extern_send(nodeinfo, sender->fd);
    return 0;
}

int promoteEXT(struct node_info* node){
    node->ext = removeContact(node->ext, node->ext);
    node->ext = node->intr;
    if(node->intr->next!=NULL){
        node->intr = node->intr->next;
    }else{
        node->intr = NULL;
    }
    return 0;
}

int new_send(int fd, char id[], char ip[], char tcp[]){
    char msg[45];
    memset(msg, 0, 45);
    sprintf(msg, "NEW %s %s %s\n", id, ip, tcp);//verificar se funcionou||||||||||||||
    write(fd, msg ,44);
    return 0;
}

int extern_send(struct node_info* nodeinfo, int fd){
    char msg[45];
    memset(msg,0,45);
    sprintf(msg, "EXTERN %s %s %s\n", nodeinfo->ext->id, nodeinfo->ext->ip, nodeinfo->ext->port);
    write(fd, msg ,44);
    return 0;
}

int query_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]){
    Contact route_dest, aux;

    //check input error|||||||||||||||||||||||

    //update routing table of sender
    if(checkRoute(nodeinfo->rout_table, origin) != NULL){//talvez apagar a entrada que ja havia e por esta atualizada|||||||(ja esta)||||||||
        nodeinfo->rout_table = removeRoute(nodeinfo->rout_table, origin);
    }
    nodeinfo->rout_table = addRoute(nodeinfo->rout_table, origin, sender);
    
    if(strcmp(nodeinfo->id, dest) == 0){//query is for this node
        if(checkName(nodeinfo->names, name) == 0){
            //envia CONTENT
            content_send(sender->fd, origin, nodeinfo->id, name);
        }
        else{
            //envia NOCONTENT
            nocontent_send(sender->fd, origin, nodeinfo->id, name);
        }
    }else{//query is NOT for this node
        //check if dest is in routing table
        if((route_dest = checkRoute(nodeinfo->rout_table, dest)) == NULL){//if not found  
            //send query to EXT
            if(nodeinfo->ext != sender){
                //send QUERY
                query_send(nodeinfo->ext->fd, dest, origin, name);
            }
            //send query to every internal neighbor
            aux = nodeinfo->intr;
            while(aux != NULL){
                if(aux != sender){
                    //send QUERY
                    query_send(aux->fd, dest, origin, name);
                }
            }
        }else{
            //send QUERY through route
            query_send(route_dest->fd, dest, origin, name);
        }
    }
    return 0;
}

int content_send(int fd, char dest[], char origin[], char name[]){
    char msg[115];
    memset(msg,0,115);
    sprintf(msg, "CONTENT %s %s %s\n", dest, origin, name);
    write(fd, msg, 115);
    return 0;
}

int nocontent_send(int fd, char dest[], char origin[], char name[]){
    char msg[117];
    memset(msg,0,117);
    sprintf(msg, "NOCONTENT %s %s %s\n", dest, origin, name);
    write(fd, msg, 117);
    return 0;
}

int query_send(int fd, char dest[], char origin[], char name[]){
    char msg[113];
    memset(msg,0,113);
    sprintf(msg, "QUERY %s %s %s\n", dest, origin, name);
    write(fd, msg, 113);
    return 0;
}

int content_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]){
    Contact route_dest, aux;

    //update routing table of sender
    if(checkRoute(nodeinfo->rout_table, origin) != NULL){
        nodeinfo->rout_table = removeRoute(nodeinfo->rout_table, origin);
    }
    nodeinfo->rout_table = addRoute(nodeinfo->rout_table, origin, sender);

    if(strcmp(nodeinfo->id, dest) == 0){//CONTENT message is for this node
        printf("%s NODE %s%s%s has name %s%s%s%s\n", BOLD, YEL, origin, WHI, YEL, name, NORM, NBOLD);
    }else{//CONTENT message is NOT for this node
        if((route_dest = checkRoute(nodeinfo->rout_table, dest)) == NULL){//if not found (will probably never happen)     
            //send CONTENT message to EXT
            if(nodeinfo->ext != sender){
                //send CONTENT
                content_send(nodeinfo->ext->fd, dest, origin, name);
            }
            //send CONTENT message to every internal neighbor
            aux = nodeinfo->intr;
            while(aux != NULL){
                if(aux != sender){
                    //send CONTENT
                    content_send(aux->fd, dest, origin, name);
                }
            }
        }else{
            //send CONTENT through route
            content_send(route_dest->fd, dest, origin, name);
        }
    }
    return 0;
}

int nocontent_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]){
    Contact route_dest, aux;

    //update routing table of sender
    if(checkRoute(nodeinfo->rout_table, origin) != NULL){
        nodeinfo->rout_table = removeRoute(nodeinfo->rout_table, origin);
    }
    nodeinfo->rout_table = addRoute(nodeinfo->rout_table, origin, sender);

    if(strcmp(nodeinfo->id, dest) == 0){//NOCONTENT message is for this node
        printf("%s NODE %s%s%s doesn't have name %s%s%s%s\n", BOLD, YEL, origin, WHI, YEL, name, NORM, NBOLD);
    }else{//NOCONTENT message is NOT for this node
        if((route_dest = checkRoute(nodeinfo->rout_table, dest)) == NULL){//if not found (will probably never happen)     
            //send NOCONTENT message to EXT
            if(nodeinfo->ext != sender){
                //send NOCONTENT
                nocontent_send(nodeinfo->ext->fd, dest, origin, name);
            }
            //send NOCONTENT message to every internal neighbor
            aux = nodeinfo->intr;
            while(aux != NULL){
                if(aux != sender){
                    //send NOCONTENT
                    nocontent_send(aux->fd, dest, origin, name);
                }
            }
        }else{
            //send NOCONTENT through route
            nocontent_send(route_dest->fd, dest, origin, name);
        }
    }
    return 0;
}