#include "contact.h"

//allocates a new contact
Contact createContact(){
    Contact temp; // declare a Contact
    temp = (Contact)malloc(sizeof(struct contact)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new Contact
}

//adds new contact to the linked list
Contact addContact(Contact head, char* id, int fd){ 
    Contact temp, p;// declare two Contacts temp and p
    temp = createContact();//createContact will return a new Contact with data = value and next pointing to NULL.
    //set variables
    temp->id = strdup(id);
    temp->fd = fd;
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

Contact removeContact(Contact head, Contact deprecated){//can be changed as to be summoned with the deprecated node's id||||||||||||||
    Contact front = head, back = NULL, new_head = head;

    while(front != NULL){
        //deprecated found
        if(strcmp(front->id, deprecated->id) == 0){
            //if deprecated is the head
            if(back == NULL){
                new_head = front->next;
            //if deprecated is the tail
            }else if(front->next == NULL){
                back->next = NULL;
            //if deprecated is on the middle
            }else{
                back->next = front->next;
            }
            close(deprecated);
            break;
        }
    }
    return new_head;
}

int fillContact(Contact contact, char* id, char* ip, char* port){
    //verify arguments
    if(verifyid(id)!=0){
        return -1;
    }
    strcpy(contact->id, id);
    strcpy(contact->ip, ip);
    strcpy(contact->port, port);
    //contact->id = strdup(id);
    //contact->ip = strdup(ip);
    //contact->port = strdup(port);
    return 0;
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

void closeContacts(Contact head){
    Contact aux = head;
    Contact aux1 = head;
    while(aux!=NULL){
        close(aux->fd);//porque close do fd?|||||||||||||||||||||||
        aux1 = aux;
        aux = aux->next;
        free(aux1);
    }
    return;
}