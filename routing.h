#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"

#define YEL "\x1b[33m"
#define WHI "\x1b[37m"
#define NORM "\x1b[m"
#define BOLD  "\e[1m"
#define NBOLD   "\e[m"

struct routing{
    char dest[3];
    Contact route;
    struct routing* next;
};

typedef struct routing* Routing;

//initialize new Routing struct
Routing createRoute(char dest[], Contact route);

//add new destination to routing table
//returns updated list head
Routing addRoute(Routing head, char dest[], Contact route);

//removes specified route from routing table
//return updated list head
Routing removeRoute(Routing head, char dest[]);

//close routing table and free the memory
void closeRoutingTable(Routing head);

//check the routing table for the intended route
//returns the neighbor that can route to the destination , if not found returns NULL
Contact checkRoute(Routing head, char dest[]);

