#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"

struct routing{
    char dest[3];
    Contact ;
    struct routing* next;
};

typedef struct routing* Routing;

//initialize new Routing struct
Routing createRoute(char dest[], char route[]);

//add new destination to routing table
//returns updated list head
Routing addRoute(Routing head, char dest[], char route[]);

//removes specified route from routing table
//return updated list head
Routing removeRoute(Routing head, char dest[]);

//close routing table and free the memory
void closeRoutingTable(Routing head);

