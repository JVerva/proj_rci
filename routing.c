#include "routing.h"


Routing createRoute(char dest[], Contact route){
    Routing temp = (Routing)malloc(sizeof(struct routing));
    strcpy(temp->dest, dest);
    temp->route = route;
    temp->next = NULL;
}

Routing addRoute(Routing head, char dest[], Contact route){
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


Contact checkRoute(Routing head, char dest[]){
    Routing aux = head;

    while(aux != NULL){
        if(strcmp(aux->dest, dest) == 0){
            return aux->route;
        }
        aux = aux->next;
    }
    return NULL;
}


void show_routing(Routing head){
    Routing aux = head;

    printf(" %s%s%s%s%s\n", BOLD, YEL, "ROUTING TABLE:", NORM, NBOLD);
    printf(" %s%-13s%s | %s%-10s%s\n", YEL, "Destination", WHI, YEL, "Neighbor", NORM);
    printf("------------------------------------------\n");

    while(aux != NULL){
        printf(" %-13s | %-10s\n",  aux->dest, aux->route->id);
    }
}

