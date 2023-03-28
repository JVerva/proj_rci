#include "names.h"

Name createName(char file[]){
    Name temp = (Name)malloc(sizeof(struct name));
    strcpy(temp->file, file);
    temp->next = NULL;
    return temp;
}

Name addName(Name head, char file[]){
    Name temp, aux;

    temp = createName(file);

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

Name removeName(Name head, char file[]){
    Name front = head, back = NULL, new_head = head;

    while(front != NULL){
        //if deprecated is found
        if(strcmp(front->file, file) == 0){
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

void closeNames(Name head){
    Name aux = head;
    Name aux1 = head;
    while(aux != NULL){
        aux1 = aux;
        aux = aux->next;
        free(aux1);
    }
    return;
}

int checkName(Name head, char file[]){
    Name aux = head;

    while(aux != NULL){
        if(strcmp(aux->file, file) == 0){
            return 0;
        }
        aux = aux->next;
    }
    return 1;
}



void show_names(Name head){
    Name aux = head;

    printf(" %s%s%s%s%s\n", BOLD, YEL, "NAMES:", NORM, NBOLD);
    while(aux != NULL){
        printf("%s\n", aux->file);
        aux = aux->next;
    }
}