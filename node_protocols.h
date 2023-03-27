#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contact.h"
#include "utils.h"
#include "names.h"
#include "routing.h"

struct node_info{
    char id[3];
    Contact ext;
    Contact bck;
    Contact intr;
    Routing rout_table;
    Name names;
};

//initializes node_info struct as the default start settings
//argument id is the Node's own id
//returns new node_info struct 
struct node_info* initNode_info();

//frees node_info struct
void closeNode_info(struct node_info *node);

//processes messages and checks if they are valid, returns message index if they are, else returns -1 and prints erros msg
//args gets filled with message arguments
int messagecheck(char buffer[], char** args);

int new_rcv(struct node_info* node, Contact sender, char id_rcv[], char ip[], char port[]);

//update network topology and node information based on incoming updates from external neighbor
int extern_rcv(struct node_info *node, char id_sender[], char id_rcv[], char ip[], char port[]);

//promote internal neighbor to ext
int promoteEXT(struct node_info* node, Contact promotee);

//send exter msg
int extern_send(struct node_info* nodeinfo, int fd);

//sends new msg
int new_send(int fd, char id[], char ip[], char tcp[]);