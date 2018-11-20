#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "list.h"

#define FALSE 0
#define TRUE !FALSE

#define MODE_D 0
#define MODE_L 1
#define MODE_F 2

int main(int argc, char **argv, char *envp[]);
int *child(int *countTo);
void assert(int beZero, char *msg);
void lockOnMode(int forMode);
void unlockOnMode(int forMode);

int mode = MODE_D;
pthread_mutex_t mutex;

int main(int argc, char **argv, char *envp[]) {
    int N = 1;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:lf")) != -1) {
        switch (opt) {
            case 'n':
                N = atoi(optarg);
                break;
            case 'l':
                mode = MODE_L;
                break;
            case 'f':
                mode = MODE_F;
                break;
        }
    }
    pthread_mutex_init(&mutex, NULL);

    list_t *threadList = list_init();

    for (int i = 0; i < N; i++) {
        int *countTo = malloc(sizeof(int));
        *countTo = i;
        pthread_t *threadId = malloc(sizeof(pthread_t));
        assert(pthread_create(threadId, NULL, (void *(*)(void *))child, countTo), "Failed to create thread.\n");
        list_append(threadList, (void *)threadId);
    }
    int *exitCode;
    while (threadList->first != NULL) {
        pthread_t *threadId = (pthread_t *)threadList->first->data;
        assert(pthread_join(*threadId, (void **) &exitCode), "Failed to join a thread.\n");
        free(threadId);
        list_remove(threadList, threadList->first);
    }
    exit(0);
}

int *child(int *k) {
    char fileName[64];
    int fileContentBuffer[64];
    unsigned fileContentBufferLength;
    int i = 0;
    sprintf(fileName, "%i.txt", *k);
    int fd = open(fileName, O_RDONLY);
    lockOnMode(MODE_F);
    while ((fileContentBufferLength = read(fd, fileContentBuffer, 63)) > 0) {
        lockOnMode(MODE_L);
        char lineStart[10];
        sprintf(lineStart, "[%02d] %03d\t", *k, i);
        write(STDOUT_FILENO, lineStart, 9);
        write(STDOUT_FILENO, fileContentBuffer, fileContentBufferLength);
        write(STDOUT_FILENO, "\n", 1);
        unlockOnMode(MODE_L);
        i++;
    }
    unlockOnMode(MODE_F);
    close(fd);
    free(k);
    return 0;
}

void assert(int beZero, char *msg) {
    if (beZero != 0) {
        printf("\n%s\n", msg);
        exit(beZero);
    }
}

void lockOnMode(int forMode) {
    if (forMode == mode) {
        assert(pthread_mutex_lock(&mutex), "Failed to lock mutex! \n");
    }
}
void unlockOnMode(int forMode) {
    if (forMode == mode) {
        assert(pthread_mutex_unlock(&mutex), "Failed to unlock mutex!\n");
    }
}
