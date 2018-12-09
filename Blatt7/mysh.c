#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"
#include <sys/resource.h>

#define FALSE 0
#define TRUE !FALSE

/**
 * This is a crocodile: >°=,^=,^-... he likes to eat the greater number, or the std in/out of a file.
 * This is a plumbus .|. it pipes pipes through pipes.
 */


typedef struct {
    char **commandList;
    char *inFileName;
    char *outFileName;
} croco_t;

char **list_to_array(list_t *list);
void tryExecute(char **argv, char *envp[]);
void knowExecute(char *path, char **argv, char *envp[]);
int main(int argc, char **argv, char *envp[]);
croco_t *crocodile(char **list);
void openFiles(croco_t *commandA, croco_t *commandB, char *envp[]);
pid_t executeCommand(char **commandList, int inFile, int outFile, int fileToClose, char *envp[]);
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
    croco->inFileName = NULL;
    croco->outFileName = NULL;
    croco->commandList = arr;
    int i = 0;
    while (arr[i] != NULL) {
        if (strncmp(arr[i], "<", 1) == 0) {
            croco->inFileName = arr[i + 1];
            free(arr[i]);
            arr[i] = NULL;
            i++;
        } else if (strncmp(arr[i], ">", 1) == 0) {
            croco->outFileName = arr[i + 1];
            free(arr[i]);
            arr[i] = NULL;
            i++;
        }
        i++;
    }
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

pid_t executeCommand(char **args, int inFile, int outFile, int fileToClose, char **envp) {
    pid_t pid = fork();
    if (0 > pid) {
        fprintf(stderr, "An error occured while trying to create a child process.\n");
        exit(-1);
    }
    if (0 < pid) {  // this is the parent process --> there's nothing left to do
        return pid;
    }
    // this is the child process

    if (0 < fileToClose) {
        close(fileToClose);
    } 

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
        int inFile = -1;
        int outFile = -1;
        if (commandA->inFileName != NULL) {
            inFile = open(commandA->inFileName, O_RDONLY);
            if (0 > inFile) {
                fprintf(stderr, "Could not open file '%s'.\n", commandA->inFileName);
                return;
            }
        }
        if (commandA->outFileName != NULL) {
            outFile = open(commandA->outFileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            if (0 > outFile) {
                fprintf(stderr, "Could not open file '%s'.\n", commandA->outFileName);
                return;
            }
        }
        executeCommand(commandA->commandList, inFile, outFile, -1, envp);
        wait(&exitCode);
        close(inFile);
        close(outFile);
    } else {
        int *files = malloc(sizeof(int) * 2);
        pipe(files);
        int inFileA = -1;
        int outFileB = -1;
        if (commandA->outFileName != NULL || commandB->inFileName != NULL) {
            fprintf(stderr, "This combination of pipes and I/O redirection is not valid.\
                    The I/O redirection will be ignored, but the pipe will be use.\n");
        }
        if (commandA->inFileName != NULL) {
            inFileA = open(commandA->inFileName, O_RDONLY);
        }
        if (commandB->outFileName != NULL) {
            outFileB = open(commandB->outFileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            printf("outFileB: %d\n", outFileB);
        }
        pid_t pidA = executeCommand(commandA->commandList, inFileA, files[1], files[0], envp);
        pid_t pidB = executeCommand(commandB->commandList, files[0], outFileB, files[1], envp);
        waitpid(pidA, &exitCode, 0);
        close(files[1]);
        waitpid(pidB, &exitCode, 0);
        close(files[0]);
        free(files);
        close(inFileA);
        close(outFileB);
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
