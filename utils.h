#include <string.h>

int verifyid(char* id);
int verifynet(char* net);
int checkfornode(char node_id[], char node_list[]);
int getnodeinfo(char node_id[], char node_list[], char ** ip, char **port);