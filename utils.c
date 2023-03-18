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

//cheks if id id valid
int verifyid(char* id){
    if(strlen(id) != 2){
        return 1;
    }else{
        for(int i = 0; i < 3; i++){
            if(!(id[i]<57 && id[i]>47)){
                return 1;
            }
        }
    }
    return 0;
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

int getnodeinfo(char node_id[], char node_list[], char ** ip, char ** port){
    //divide buffer into tokens
    //extract the first token
    char* token = strtok(node_list, " \n");
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
                return 0;
            }
        }
    }
    return -1;
}

