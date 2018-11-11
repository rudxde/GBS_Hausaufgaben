#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "list.h"


#define FALSE 0
#define TRUE !FALSE

int main(int argc, char **argv, char *envp[]);
void parent(int pid);
void child(int K, int REnabled);
void printDate(char prefix[]);
int randomNext(int min, int max);

int main(int argc, char **argv, char *envp[])
{
    int K = 10;
    int N = 1;
    int R = FALSE;
    int opt = 0;
    while ((opt = getopt(argc, argv, "k:n:r")) != -1)
    {
        switch (opt)
        {
        case 'k':
            K = atoi(optarg);
            break;
        case 'n':
            N = atoi(optarg);
            break;
        case 'r':
            R = TRUE;
            break;
        }
    }

    list_t * childList = list_init();

    // set's the seed for the random function
    srand(time(NULL));
    printDate("Start:");

    for (int i = 0; i < N; i++)
    {
        int countTo = R ? randomNext(K / 2, 1.5 * K) : K;
        int pid = fork();
        if (pid == -1)
        {
            printf("childprocess could not be created!\n");
            exit(-1);
        }
        else if (pid == 0)
        {
            // Im the child
            child(countTo);
        }
        int * pidPointer = malloc(sizeof(int));
        *pidPointer = pid;
        list_append(childList, pidPointer);
    }
    int exitCode = 0;
    while(childList->first != NULL) {
        int * childPid = childList->first->data;
        waitpid(*childPid ,&exitCode, 0);
        free(childPid);
        printf("Exit-Code: %i\n", WEXITSTATUS(exitCode));
        list_remove(childList, childList->first);
    }

    printDate("Ende:");
    exit(0);
}


void child(int countTo)
{
    int myPid = getpid();
    int parentPid = getppid();
    // int countTo = randomNext(K/2, 1.5*K);
    for (unsigned i = 1; i <= countTo; i++)
    {
        sleep(1);
        printf("%6i %6i %u\n", myPid, parentPid, i);
    }
    exit((myPid + countTo) % 100);
}

void printDate(char prefix[])
{
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);
    printf("%s %s", prefix, asctime(timeinfo));
}

int randomNext(int min, int max)
{
    return (rand() % (max - min)) + min;
}
