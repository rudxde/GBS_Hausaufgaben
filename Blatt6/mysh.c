#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "parser.h"


char ** list_to_array(list_t *list){
    int length = 0;
    list_elem_t * head = list->first; 
    while(head != NULL){
        length++;
        head = head->next;
    }
    char ** array = malloc(sizeof(char *) * (length+1));
    head = list->first;
    int i = 0;
    for(i = 0; i < length; i++){
        array[i] = (char *)head;
        head = head->next;
    }
    array[length] = NULL;

    return array;

    
}

int main(int argc, char **argv, char *envp[]) {
}