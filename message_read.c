int process_stream(char* stored, char* msg){
    char buffer[100];
    char *aux;
    //int MAX_MSG = 117;
    int max_msg = 10;

    memcpy(buffer, stored, 100);

    if(strlen(stored) != 0){
        aux = strtok(buffer, "\n");
        strcpy(msg, aux);
    }
    else{
        return -1;
    }

    if(strlen(msg) > max_msg-1){
        //discard
        memset(stored, '\0', 100);
    }
    else if(strlen(msg) < strlen(stored)){  //found a complete message with reasonable size
        //reattach \n symbol to msg (strtok removed it)
        printf("papawk,\n");
        strcat(msg, "\n");
        //shift left in stored
        memmove(stored, &stored[strlen(msg)], 100-strlen(msg));

        return 0;
    }
    else if(strlen(msg) == strlen(stored)){ //incomplete message
        //wait for complete??||||||||||||||
    }
    return -1;
}