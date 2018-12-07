#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define FALSE 0
#define TRUE !FALSE

/**
 * This is a crocodile: >°=,^=,^-... he likes to eat the greater number, or the std in/out of a file.
 * This is a plumbus .|. it pipes pipes through pipes.
 */

char **list_to_array(list_t *list);
void tryExecute(char **argv, char *envp[]);
void knowExecute(char *path, char **argv, char *envp[]);
int main(int argc, char **argv, char *envp[]);
croco_t *crocodile(list_t *list);
void openFiles(croco_t *commandA, croco_t *commandB, char *envp[]);
void executeComand(list_t * commandList, FILE inFile, FILE outFile, char *envp[]);
void plumbus(list_t *listO, char *envp[]);


typedef enum { none = -1, crocoEatsStdId = 0; crocoEatsStdOut = 1; }
crocoType_t;

typedef struct {
    list_t commandList;
    crocoType_t type;
    char *fileName;
} croco_t;

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
        array[i] = (char *)head->data;
        head = head->next;
    }
    array[length] = NULL;
    return array;
}

void parse_command(char **list, char *envp[]) {
    char *commandpath = list[0];
    // Explizite Kommandopfad Angabe
    if (strchr(commandpath, '/') != NULL) {
        knowExecute(commandpath, list, envp);
    } else {
        tryExecute(list, envp);
    }
}

void tryExecute(char **argv, char *envp[]) {
    char *path = getVariableValue(envp, "PATH");
    char *delimiter = ":";
    char *tryPath = strtok(path, delimiter);
    char *commandWithPath = malloc(sizeof(char) * 1024);
    strcpy(commandWithPath, tryPath);
    strcat(commandWithPath, "/");
    strcat(commandWithPath, argv[0]);
    int status;
    do {
        pid_t pid = fork();
        if (0 == pid) {  // this is the child process -> execute the command
            execve(commandWithPath, argv, envp);
            exit(-1);
        }
        // this is the parent process -> wait for the child process to finish
        if (0 > wait(&status)) {
            fprintf(stderr, "An error has occured while waiting for a child thread.\n");
            exit(-1);
        }
        tryPath = strtok(NULL, delimiter);
        if (tryPath == NULL) {
            printf("cannot execute '%s'\n", argv[0]);
            free(commandWithPath);
            return;
        }
        strcpy(commandWithPath, tryPath);
        strcat(commandWithPath, "/");
        strcat(commandWithPath, argv[0]);

    } while (0 != WEXITSTATUS(status));
}

void knowExecute(char *path, char **argv, char *envp[]) {
    pid_t pid = fork();
    if (0 == pid) {  // this is the child process -> execute the command
        int status = execve(path, argv, envp);
        if (0 > status) {
            printf("cannot execute '%s'\n", argv[0]);
        }
        exit(status);
    }
    if (0 > wait(NULL)) {
        fprintf(stderr, "An error has occured while waiting for a child thread.\n");
        exit(-1);
    }
    return;
}

croco_t * crocodile(list_t * list) {

}

void plumbus(list_t *listO) {
    for (list_elem_t actualElement = listO->first; actualElement != NULL; actualElement = actualElement->next) {
        if (strncmp((char *)actualElement->next->data, "|", 1)) {
            list_t * listA = list_init();
            list_t * listB = list_init();
            listA->first = listO->first;
            listA->last = actualElement;
            actualElement->next = NULL;
            listB->first = actualElement->next->next;
            listB->last = listO->last;


            return;
        }
    }
}

int main(int argc, char **argv, char *envp[]) {
    setbuf(stdout, NULL);
    char input[1024];

    while (TRUE) {
        printf("$ ");
        fgets(input, 1024, stdin);
        list_t *commandList = parse(input, envp);
        if (NULL == commandList->first) {
            continue;
        }
        if (strncmp((char *)commandList->first->data, "exit", 4) == 0) exit(0);
        char **args = list_to_array(commandList);
        parse_command(args, envp);
    }
}
