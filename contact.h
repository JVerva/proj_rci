#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

struct contact{
    char id[3];
    int fd;
    char ip[17];
    char port[6];
    struct contact *next;
};

typedef struct contact* Contact;

//allocates a new contact
Contact createContact();

//adds new contact to the linked list
Contact addContact(Contact head, char* id, int fd);

//removes contact from list and frees memory
Contact removeContact(Contact head, Contact deprecated);

//return contact with corresponding id from contact list
Contact getContact(Contact head, char* id);

//fills struct with Contact information 
int fillContact(Contact contact, char* id, char* ip, char* port);

//closes fds and frees contact list
void closeContacts(Contact head);