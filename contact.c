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

Contact createContact(){
    Contact temp; // declare a Contact
    temp = (Contact)malloc(sizeof(struct contact)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new Contact
}

Contact addContact(Contact head, char* id, int fd, char* ip, char* port){ 
    Contact temp,p;// declare two Contacts temp and p
    temp = createContact();//createContact will return a new Contact with data = value and next pointing to NULL.
    //set variables
    temp->id = strdup(id);
    temp->fd = fd;
    temp->ip = strdup(ip);
    temp->port = strdup(port);
    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last Contact.The last Contact always points to NULL.
        }
        p->next = temp;//Point the previous last Contact to the new Contact created.
    }
    return head;
}

//return contact with corresponding id from contact list
Contact getContact(Contact head, char* id){
    Contact temp;
    for(temp = head; strcmp(temp->id, id)!= 0; temp = temp->next){
        if(temp->next == NULL && strcmp(temp->id, id)!= 0){
            return NULL;
        }
    }
    return temp;
}

