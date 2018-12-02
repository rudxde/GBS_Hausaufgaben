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
void parse_command(char **list, char *envp[]){
    char *commandpath = list[0];
    char * command;
    //Explizite Kommandopfad Angabe
    if(strchr(commandpath, '/') != NULL){
        char * pointer = strrchr(commandpath, '/')[0] 
        pointer[0] = NULL;
        command = *pointer[1];
    }else{
        command = commandpath;
    }
}
int main(int argc, char **argv, char *envp[]) {
}