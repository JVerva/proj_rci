#include "node_protocols.h"

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

int messagecheck(char buffer[], char** args){
    int index = 0;
    int n = -1;
    char msg[10];

    //divide buffer into tokens
    //extract the first token
    char* token = strtok(buffer, " \n");
    if(token != NULL){
        strcpy(msg, token);
    }
    //loop through the string to extract all other tokens
    for(int i=0; token != NULL; i++ ) {
        token = strtok(NULL, " \n");
        if(i>2){
            return -1;
        }
        args[i] = token;
        n++;
    }

    //do all checks
    if(strcmp(msg, MSGS[0])==0){
        if(n!=3){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 0;
        }
    }else if(strcmp(msg, MSGS[1])==0){
        if(n!=3){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 1;
        }
    }else if(strcmp(msg, MSGS[2])==0){
        if(n!=1){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 2;
        }
    }else if(strcmp(msg, MSGS[3])==0){
        if(n!=3){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 3;
        }
    }else if(strcmp(msg, MSGS[4])==0){
        if(n!=3){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 4;
        }
    }else if(strcmp(msg, MSGS[5])==0){
        if(n!=3){
            //fprintf(stderr, "%s error: wrong number of arguments.\n", MSGS[0]);
            index = -1;
        }else{
            index = 5;
        }
    }else{
        //fprintf(stderr, "error: %s is not a valid message.\n", msg);
        return -1;
    }
    return index;
}

int extern_rcv(struct node_info *node, char id[], char ip[], char port[]){
    //maybe check for input error|||||||||||||||||||||||||||

    //check if message is coming from ext neighbor, else do nothing
    if(strcmp(node->ext->id, id) != 0){
        return -1;
    }else{
        //update backup neighbor contact information
        strcpy(id, node->bck->id);
        strcpy(ip, node->bck->ip);
        strcpy(port, node->bck->port);
        strcpy("-1", node->bck->fd);//|||||||||||

        //UPDATE ROUTING TABLE?|||||||||||||
    }
}