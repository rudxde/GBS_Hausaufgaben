#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "parser.h"


(*void)[] list_to_array(list_t list){
    int length = 0;
    list_element_t head = list->first; 
    while(head != null){
        length++;
        head = head->next;
    }
}
int main(int argc, char **argv, char *envp[]) {
}