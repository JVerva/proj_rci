#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct contact{
    char* id;
    int fd;
    char *ip;
    char *port;
    struct contact *next;
};

typedef struct contact* Contact;

struct routing{
    char *dest;
    Contact route;
    struct routing *next;
};

struct node_info{
    char *id;
    Contact ext;
    Contact bck;
    Contact intr;
    struct routing *rout_table;
};

//allocates a new contact
Contact createContact();

//adds new contact to the linked list
Contact addContact(Contact head, char* id, int fd, char* ip, char* port);

//return contact with corresponding id from contact list
Contact getContact(Contact head, char* id);

