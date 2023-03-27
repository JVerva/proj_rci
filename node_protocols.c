#include "node_protocols.h"

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

struct node_info* initNode_info(){
    struct node_info *temp = (struct node_info*)malloc(sizeof(struct node_info));
    strcpy(temp->id, "-1");
    temp->intr = NULL;
    temp->ext = NULL;//must be closed|||||||||||||||
    temp->bck = NULL;
    temp->rout_table = NULL;
    return temp;
}

void closeNode_info(struct node_info *node){
    removeContact(node->bck,node->bck);
    removeContact(node->ext,node->ext);
    closeContacts(node->intr);
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
        promoteEXT(nodeinfo, sender);
    //else its a new internal neighbor
    }else{
    }
    extern_send(nodeinfo, sender->fd);
    return 0;
}

int promoteEXT(struct node_info* node, Contact promotee){
    fillContact(node->ext, promotee->id, promotee->ip, promotee->port);
    node->ext->fd = promotee->fd;
    promotee->fd = -1;
    node->intr = removeContact(node->intr, promotee);
    return 0;
}

int new_send(int fd, char id[], char ip[], char tcp[]){
    char msg[44];
    memset(msg, 0, 44);
    sprintf(msg, "NEW %s %s %s", id, ip, tcp);//verificar se funcionou||||||||||||||
    write(fd, msg ,44);
    return 0;
}

int extern_send(struct node_info* nodeinfo, int fd){
    char msg[44];
    memset(msg,0,44);
    sprintf(msg, "EXTERN %s %s %s", nodeinfo->ext->id, nodeinfo->ext->ip, nodeinfo->ext->port);
    write(fd, msg ,44);
    return 0;
}

int query_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]){
    Contact route_dest, aux;

    //check input error|||||||||||||||||||||||

    //update routing table of sender
    if(checkRoute(nodeinfo->rout_table, origin) == NULL){
        nodeinfo->rout_table = addRoute(nodeinfo->rout_table, origin, sender);
    }
    
    if(strcmp(nodeinfo->id, dest) == 0){//query is for this node
        if(checkName(nodeinfo->names, name) == 0){
            //envia CONTENT|||||||||||||
        }
        else{
            //envia NOCONTENT
        }
    }else{//query is NOT for this node
        //check if dest is in routing table
        if(route_dest = checkRoute(nodeinfo->rout_table, dest) == NULL){//if not found        
            //send query to EXT
            if(nodeinfo->ext != sender){
                //send QUERY||||||||||||||||||
            }
            //send query to every internal neighbor
            aux = nodeinfo->intr;
            while(aux != NULL){
                if(aux != sender){
                    //send QUERY|||||||||||||||||
                }
            }
        }else{
            //send QUERY through route||||||||||||
        }
    }
    return 0;
}

int content_send(struct node_info* nodeinfo, char name, ){}

int query_send(struct node_info* nodeinfo, )