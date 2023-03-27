#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct name{
    char file[100];
    struct name* next;
};

#define YEL "\x1b[33m"
#define WHI "\x1b[37m"
#define NORM "\x1b[m"
#define BOLD  "\e[1m"
#define NBOLD   "\e[m"

typedef struct name* Name;

//initialize "name" struct and fill data
Name createName(char file[]);

//create name struct and insert in list
//returns updated list head
Name addName(Name head, char file);

//remove name from list
//returns updated list head
Name removeName(Name head, char file[]);


//close routing table and free the memory
void closeNames(Name head);

//check if intended name is in the name list
//returns 0 if true, 1 if false
int checkName(Name head, char file[]);

//print to terminal tha list of names
void show_names(Name head);