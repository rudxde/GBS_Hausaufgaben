#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define FALSE 0
#define TRUE !FALSE

int main(int argc, char **argv, char *envp[]) {
    char input[1024];
    unsigned running = TRUE;
    while (running) {
        printf("$ ");
        fgets(input, 1024, stdin);
        list_t *commandList = parse(input, envp);
        list_elem_t *currentElement = commandList->first;
        if (strncmp((char *)currentElement->data, "exit", 4) == 0) running = FALSE;
        for (unsigned i = 1; currentElement != NULL; i++) {
            printf("%u:%s\n", i, (char *)currentElement->data);
            free(currentElement->data);
            currentElement = currentElement->next;
        }
    }
}
