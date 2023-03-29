int read_stream(char* stored, char *read){
    

    //stored is double the size of read
    strcat(stored, read);


}

int process_stream(char* stored, char* msg){
    char buffer[240];
    int reading = 1;
    int i;
    int MAX_MSG = 116;

    memcpy(buffer, stored, sizeof(buffer));

    while(reading){
        if(stored[0] == "\0"){
            //nothing to read
            return -1;
        }
        for(i = 0; i < MAX_MSG-1; i++){
            if(stored[i] == "\n"){
                //message complete and ready to be read
                msg = strtok(buffer, "\n");
                //remove read msg from stored
                memmove(stored, stored[i], strlen(msg)-(i+1));
                return 0;
            }
            else if(stored[i] == "\0"){
                //message not yet complete
                return -1;
            }
        }
        if(i > MAX_MSG-1){
            //maximum message lenght is 116 so this cant be a legitimate message
            //remove confirmed trash from stored
            memmove(stored, stored[i], sizeof(buffer)-MAX_MSG);
        }
    }
}