#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* MSGS[] = {"NEW", "EXTERN", "WITHDRAW", "QUERY", "CONTENT", "NOCONTENT"}; 

//processes messages and checks if they are valid, returns message index if they are, else returns -1 and prints erros msg
//args gets filled with message arguments
int messagecheck(char buffer[], char** args);
