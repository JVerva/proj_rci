
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contact.h"

struct node_info{
    char *id;
    Contact ext;
    Contact bck;
    Contact intr;
    struct routing *rout_table;
};

//processes messages and checks if they are valid, returns message index if they are, else returns -1 and prints erros msg
//args gets filled with message arguments
int messagecheck(char buffer[], char** args);

//update network topology and node information based on incoming updates from external neighbor
int extern_rcv(struct node_info *node, char id_sender[], char id_rcv[], char ip[], char port[]);