#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

#define FALSE 0
#define TRUE !FALSE

char **list_to_array(list_t *list);
void tryExecute(char **argv, char *envp[]);
void knowExecute(char *path, char **argv, char *envp[]);
int main(int argc, char **argv, char *envp[]);

char **list_to_array(list_t *list) {
    int length = 0;
    list_elem_t *head = list->first;
    while (head != NULL) {
        length++;
        head = head->next;
    }
    char **array = malloc(sizeof(char *) * (length + 1));
    head = list->first;
    int i = 0;
    for (i = 0; i < length; i++) {
        array[i] = (char *)head;
        head = head->next;
    }
    array[length] = NULL;

    return array;
}
void parse_command(char **list, char *envp[]) {
    char *commandpath = list[0];
    // Explizite Kommandopfad Angabe
    if (strchr(commandpath, '/') != NULL) {
        char *pointer = strrchr(commandpath, '/');
        pointer[0] = '\0';
        list[0] = pointer[1];
        knowExecute(commandpath, list, envp);
    } else {
        tryExecute(list, envp);
    }
}

void tryExecute(char **argv, char *envp[]) {
    char *path = getVariableValue(envp, "PATH");
    char *delimiter = ":";
    char *tryPath = strtok(path, delimiter);
    while (execve(tryPath, argv, envp) < 0) {
        tryPath = strtok(NULL, delimiter);
        if (tryPath == NULL) {
            printf("cannot execute '%s'", argv[0]);
        }
    }
}

void knowExecute(char *path, char **argv, char *envp[]) {
    if (execve(path, argv, envp) < 0) {
        printf("cannot execute '%s'", argv[0]);
    }
}

int main(int argc, char **argv, char *envp[]) {
    char input[1024];

    while (TRUE) {
        printf("$ ");
        fgets(input, 1024, stdin);
        list_t *commandList = parse(input, envp);
        char **args = list_to_array(commandList);
        if (strncmp(args[0], "exit", 4) == 0) exit(0);
        parse_command(args, envp);
    }
}