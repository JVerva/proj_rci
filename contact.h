#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct contact{
    char* id;
    int fd;
    char *ip;
    char *port;
    struct contact *next;
};

typedef struct contact* Contact;

//allocates a new contact
Contact createContact();

//adds new contact to the linked list
Contact addContact(Contact head, char* id, int fd);

//return contact with corresponding id from contact list
Contact getContact(Contact head, char* id);

//fills struct with Contact information 
void fillContact(Contact contact, char* id, char* ip, char* port);

//closes fds and frees contact list
void closeContacts(Contact head);