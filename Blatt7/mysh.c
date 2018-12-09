#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define FALSE 0
#define TRUE !FALSE

/**
 * This is a crocodile: >°=,^=,^-... he likes to eat the greater number, or the std in/out of a file.
 * This is a plumbus .|. it pipes pipes through pipes.
 */

typedef enum { none = -1, crocoEatsStdIn = 0, crocoEatsStdOut = 1 } crocoType_t;

typedef struct {
    char **commandList;
    crocoType_t type;
    char *fileName;
} croco_t;

char **list_to_array(list_t *list);
void tryExecute(char **argv, char *envp[]);
void knowExecute(char *path, char **argv, char *envp[]);
int main(int argc, char **argv, char *envp[]);
croco_t *crocodile(char **list);
void openFiles(croco_t *commandA, croco_t *commandB, char *envp[]);
pid_t executeCommand(char **commandList, int inFile, int outFile, char *envp[]);
void plumbus(char **list, char *envp[]);
void freeStringArray(char ** array);

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

void tryExecute(char **argv, char *envp[]) {
    char *path = getVariableValue(envp, "PATH");
    char *delimiter = ":";
    char *tryPath = strtok(path, delimiter);
    char *commandWithPath = malloc(sizeof(char) * 1024);
    while (NULL != tryPath) {
        strcpy(commandWithPath, tryPath);
        strcat(commandWithPath, "/");
        strcat(commandWithPath, argv[0]);
        execve(commandWithPath, argv, envp);
        tryPath = strtok(NULL, delimiter);
    }
    free(commandWithPath);
    fprintf(stderr, "Cannot execute '%s'.\n", argv[0]);
    exit(-1);
}

void knowExecute(char *path, char **argv, char *envp[]) {
    execve(path, argv, envp);
    printf("Cannot execute '%s'.\n", argv[0]);
    exit(-1);
}

croco_t *crocodile(char **arr) {
    croco_t *croco = malloc(sizeof(croco_t));
    int i = 0;
    while (arr[i] != NULL) {
        if (strncmp(arr[i], "<", 1) == 0) {
            croco->type = crocoEatsStdIn;
            croco->fileName = arr[i + 1];
            arr[i] = NULL;
            croco->commandList = arr;
            return croco;
        } else if (strncmp(arr[i], ">", 1) == 0) {
            croco->fileName = arr[i + 1];
            arr[i] = NULL;
            croco->type = crocoEatsStdOut;
            croco->commandList = arr;
            return croco;
        }
        i++;
    }
    croco->fileName = NULL;
    croco->commandList = arr;
    croco->type = none;
    return croco;
}

void plumbus(char **listO, char *envp[]) {
    for (int i = 0; listO[i] != NULL; i++) {
        if (strncmp(listO[i], "|", 1) == 0) {
            free(listO[i]);
            listO[i] = NULL;
            char **listB = &listO[i + 1];
            croco_t *commandA = crocodile(listO);
            croco_t *commandB = crocodile(listB);
            openFiles(commandA, commandB, envp);
            freeStringArray(listO);
            freeStringArray(listB);
            free(listO);
            return;
        }
    }
    openFiles(crocodile(listO), NULL, envp);
    freeStringArray(listO);
    free(listO);
}

pid_t executeCommand(char **args, int inFile, int outFile, char **envp) {
    pid_t pid = fork();
    if (0 > pid) {
        fprintf(stderr, "An error occured while trying to create a child process.\n");
        exit(-1);
    }
    if (0 < pid) {  // this is the parent process --> there's nothing left to do
        return pid;
    }
    // this is the child process

    if (0 < inFile) {   // replace stdin with the input file 
        close(0);
        if (0 != dup(inFile)) {
            fprintf(stderr, "Failed to change input file of child process.\n");
        }
        close(inFile);
    }
    if (0 < outFile) {  // replace stdout with the output file
        close(1);
        if (1 != dup(outFile)) {
            fprintf(stderr, "Failed to change output file of child process.\n");
        }
        close(outFile);
    }
    
    char *commandpath = args[0];
    // Explizite Kommandopfad Angabe
    if (strchr(commandpath, '/') != NULL) {
        knowExecute(commandpath, args, envp);
    } else {
        tryExecute(args, envp);
    }
    
    //this will never happen, but this line is necessary to keep the compiler from printing a warning
    return 0;
}

void freeStringArray(char ** array) {
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }
}

void openFiles(croco_t *commandA, croco_t *commandB, char *envp[]) {
    int exitCode = 0;
    if (commandB == NULL) {
        if (commandA->type == none) {
            executeCommand(commandA->commandList, -1, -1, envp);
        } else if (commandA->type == crocoEatsStdIn) {
            int newFile = open(commandA->fileName, O_RDONLY);
            if (0 > newFile) {
                fprintf(stderr, "Could not open file '%s'.\n", commandA->fileName);
                return;
            }
            executeCommand(commandA->commandList, newFile, -1, envp);
        } else {
            int newFile = open(commandA->fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            if (0 > newFile) {
                fprintf(stderr, "Could not open file '%s'.\n", commandA->fileName);
                return;
            }
            executeCommand(commandA->commandList, -1, newFile, envp);
        }
        wait(&exitCode);
    } else {
        int *files = malloc(sizeof(int) * 2);
        pipe(files);
        int inFileA = -1;
        int outFileB = -1;
        if (commandA->type == crocoEatsStdIn) {
            inFileA = open(commandA->fileName, O_RDONLY);
        }
        if (commandB->type == crocoEatsStdOut) {
            outFileB = open(commandA->fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        }
        pid_t pidA = executeCommand(commandA->commandList, inFileA, files[1], envp);
        pid_t pidB = executeCommand(commandB->commandList, files[0], outFileB, envp);
        waitpid(pidA, &exitCode, 0);
        close(files[1]);
        waitpid(pidB, &exitCode, 0);
        free(files);
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
        plumbus(args, envp);
    }
}
