#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contact.h"
#include "utils.h"

struct node_info{
    char id[3];
    Contact ext;
    Contact bck;
    Contact intr;
    struct routing *rout_table;
};

//initializes node_info struct as the default start settings
//argument id is the Node's own id
//returns new node_info struct 
struct node_info* initNode_info(char id[]);

//frees node_info struct
void closeNode_info(struct node_info *node);

//processes messages and checks if they are valid, returns message index if they are, else returns -1 and prints erros msg
//args gets filled with message arguments
int messagecheck(char buffer[], char** args);

//update network topology and node information based on incoming updates from external neighbor
int extern_rcv(struct node_info *node, char id_sender[], char id_rcv[], char ip[], char port[]);


int promoteEXT(struct node_info* node, Contact promotee);