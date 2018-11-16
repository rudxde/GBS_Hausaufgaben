#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "list.h"

#define FALSE 0
#define TRUE !FALSE

int main(int argc, char **argv, char *envp[]);
int *child(int *countTo);
void printDate(char prefix[]);
int randomNext(int min, int max);

int main(int argc, char **argv, char *envp[]) {
    int err;  // used to store error codes

    int K = 10;
    int N = 1;
    int R = FALSE;
    int opt = 0;
    while ((opt = getopt(argc, argv, "k:n:r")) != -1) {
        switch (opt) {
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

    list_t *threadList = list_init();

    // set's the seed for the random function
    srand(time(NULL));
    printDate("Start:");

    for (int i = 0; i < N; i++) {
        int *countTo = malloc(sizeof(int));
        *countTo = R ? randomNext(K / 2, 1.5 * K) : K;
        pthread_t *threadId = malloc(sizeof(pthread_t));
        if (0 != (err = pthread_create(threadId, NULL, &child, countTo))) {
            fprintf(stderr, "Failed to create thread.\n");
            exit(err);
        }
        list_append(threadList, (void *) threadId);
    }
    int *exitCode;
    while (threadList->first != NULL) {
        pthread_t *threadId = threadList->first->data;
        if (0 != (err = pthread_join(*threadId, &exitCode))) {
            fprintf(stderr, "Failed to join a thread.\n");
            exit(err);
        }
        free(threadId);
        // printf("Exit-Code: %i\n", WEXITSTATUS(**exitCode));
        free(exitCode);
        list_remove(threadList, threadList->first);
    }
    printDate("Ende:");
    exit(0);
}

int *child(int *countTo) {
    unsigned int threadId = (unsigned int)pthread_self();
    int pid = getpid();
    // int countTo = randomNext(K/2, 1.5*K);
    for (int i = 1; i <= *countTo; i++) {
        sleep(1);
        printf("%10u\t%d\t%u\n", threadId, pid, i);
    }

    // I think we actually don't need to worry about the exit status this time, but whatever
    int *status = malloc(sizeof(int));
    *status = (threadId + *countTo) % 100;
    free(countTo);
    return status;  // same as calling pthread_exit
}

void printDate(char prefix[]) {
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);
    printf("%s %s", prefix, asctime(timeinfo));
}

int randomNext(int min, int max) { return (rand() % (max - min)) + min; }
