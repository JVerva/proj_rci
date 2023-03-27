#include "routing.h"


Routing createRoute(char dest[], char route[]){
    Routing temp = (Routing)malloc(sizeof(Routing));
    strcpy(temp->dest, dest);
    strcpy(temp->route, route);
    temp->next = NULL;
}

Routing addRoute(Routing head, char dest[], char route[]){
    Routing temp, aux;

    temp = createRoute(dest, route);

    if(head == NULL){
        head = temp;
    }else{
        aux = head;
        while(aux->next != NULL){
            aux = aux->next;
        }
        aux->next = temp;
    }
    return head;
}

Routing removeRoute(Routing head, char dest[]){
    Routing front = head, back = NULL, new_head = head;

    while(front != NULL){
        //if deprecated is found
        if(strcmp(front->dest, dest) == 0){
            //if deprecated is the head
            if(back == NULL){
                new_head = front->next;
            //if deprecated is the tail
            }else if(front->next == NULL){
                back->next = NULL;
            //if deprecated is in the middle
            }else{
                back->next = front->next;
            }
            free(front);
            break;
        }
        back = front;
        front = front->next;
    }
    return new_head;
}

void closeRoutingTable(Routing head){
    Routing aux = head;
    Routing aux1 = head;
    while(aux != NULL){
        aux1 = aux;
        aux = aux->next;
        free(aux1);
    }
    return;
}