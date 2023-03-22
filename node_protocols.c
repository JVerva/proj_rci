#include "node_protocols.h"

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

struct node_info* initNode_info(char id[]){
    struct node_info *temp = (struct node_info*)malloc(sizeof(struct node_info));
    temp->ext = createContact();//must be closed|||||||||||||||
    strcpy(temp->ext->id, id);
    temp->bck = createContact();
    strcpy(temp->bck->id, id);
    return temp;
}

void closeNode_info(struct node_info *node){
    free(node->ext);
    free(node->bck);
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

int new_rcv(struct node_info* node, Contact sender, char id_rcv[], char ip[], char port[]){
    //INPUT ERROR CHECK||||||||||||||||||
    
    //unknown contact
    if(strcmp(sender->id, "-1") == 0){
        fillContact(sender, id_rcv, ip, port);
    }
    //if node is alone, incoming node becomes its ext
    if(strcmp(node->id, node->ext->id) == 0){
        promoteEXT(node, sender);
    //else its a new internal neighbor
    }else{
        //send EXTERN message
    }
    return 0;
}


int promoteEXT(struct node_info* node, Contact promotee){
    fillContact(node->ext, promotee->id, promotee->ip, promotee->port);
    removeContact(node->intr, promotee);
    return 0;
}