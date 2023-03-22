#include "node_protocols.h"

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

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