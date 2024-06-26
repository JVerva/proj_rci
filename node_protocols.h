#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "names.h"
#include "routing.h"

struct node_info{
    char id[3];
    char ip[18];
    char port[7];
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
int promoteEXT(struct node_info* node);

//send exter msg
int extern_send(struct node_info* nodeinfo, int fd);

//sends new msg
int new_send(int fd, char id[], char ip[], char tcp[]);

//receive query, update routing table and spread across network or through routing table entry
int query_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]);

//send CONTENT message to fd
int content_send(int fd, char dest[], char origin[], char name[]);

//send NOCONTENT message to fd
int nocontent_send(int fd, char dest[], char origin[], char name[]);

//send QUERY message to fd
int query_send(int fd, char dest[], char origin[], char name[]);

//receive CONTENT, update routing table and spread across network or through routing table entry
int content_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]);

//receive NOCONTENT, update routing table and spread across network or through routing table entry
int nocontent_rcv(struct node_info* nodeinfo, Contact sender, char dest[], char origin[], char name[]);

//sends withdraw msg
int withdraw_send(int fd, char id[]);

//removes all instances of id in routing table
int withdraw_rcv(struct node_info* nodeinfo, Contact sender, char id[]);
