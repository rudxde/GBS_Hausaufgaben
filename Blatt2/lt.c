#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

#define FALSE 0
#define TRUE !FALSE

void print_string(char * string);
int stringcomprare(char const* _Str1, char const* _Str2);

int main(int argc, char *argv [], char *envp[]) {
    list_t * li;
    if((li = list_init()) == NULL) {
        perror("Cannot_allocate_memory\n");
        exit(-1);
    }
    for(unsigned i = 1; i < argc; i++) {
        if (strcmp("-i", argv[i]) == 0) {
            if(list_insert(li, argv[++i]) == NULL) {
                perror("Cannot_allocate_memory\n");
            }
        } else if (strcmp("-a", argv[i]) == 0) {
            if(list_append(li, argv[++i]) == NULL) {
                perror("Cannot_allocate_memory\n");
            }
        } else if (strcmp("-r", argv[i]) == 0) {
            struct list_elem * toRemove = list_find(li, argv[++i], stringcomprare);
            if(toRemove == NULL) {
                perror("Cannot find element\n");
            } else {
                list_remove(li, toRemove);
            }
        } else {
            perror("unknown ");
            perror(argv[i]);
            perror("'\n");
        }
    }
    list_print(li, (void *) print_string);
    exit(0);
}

int stringcomprare(char const* _Str1, char const* _Str2) {
    return strcmp(_Str1, _Str2) == 0 ? 0 : - 1;
}

void print_string(char * string) {
    printf("%s", string);
}
