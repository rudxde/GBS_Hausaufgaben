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
#define MODE_O 3

int main(int argc, char **argv, char *envp[]);
int *child(int *countTo);
int write_buffer(long thread, char *buffer, int len);
void assert(int beZero, char *msg);
void lockOnMode(int forMode);
void unlockOnMode(int forMode);
void updateCondOnMode(int forMode, long nextThread);

int mode = MODE_D;
pthread_mutex_t write_mutex, cond_mutex;

pthread_cond_t cond;
long currentThread = 0;

int main(int argc, char **argv, char *envp[]) {
    int N = 1;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:lfo")) != -1) {
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
            case 'o':
                mode = MODE_O;
                break;
        }
    }
    pthread_mutex_init(&write_mutex, NULL);
    pthread_mutex_init(&cond_mutex, NULL);

    list_t *threadList = list_init();

    assert(pthread_cond_init(&cond, NULL), "Failed to create phtread conditional variable.\n");

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
    char fileContentBuffer[64];
    unsigned fileContentBufferLength;
    int i = 0;
    sprintf(fileName, "%i.txt", *k);
    int fd = open(fileName, O_RDONLY);
    lockOnMode(MODE_F);
    while ((fileContentBufferLength = read(fd, fileContentBuffer, 63)) > 0) {
        lockOnMode(MODE_L);
        char lineStart[10];
        sprintf(lineStart, "[%02d] %03d\t", *k, i);
        write_buffer((long) *k, lineStart, 9);
        write_buffer((long) *k, fileContentBuffer, fileContentBufferLength);
        write_buffer((long) *k, "\n", 1);
        unlockOnMode(MODE_L);
        i++;
    }
    unlockOnMode(MODE_F);
    updateCondOnMode(MODE_O, *k + 1);
    close(fd);
    free(k);
    return 0;
}

int write_buffer(long thread, char *buffer, int len) {
    _Bool ready = mode != MODE_O;
    if (!ready) {
        assert(pthread_mutex_lock(&cond_mutex), "Failed to lock mutex before checking if it is this thread's turn to write.\n");
    }
    while (!ready) {
        ready = (thread == currentThread);
        if (!ready) {
            assert(pthread_cond_wait(&cond, &cond_mutex), "Failed to wait for pthread condition.\n");
        } else {
            assert(pthread_mutex_unlock(&cond_mutex), "Failed to unlock mutex after checking if it is this thread's turn to write.\n");
        }
    }
    return write(STDOUT_FILENO, buffer, len);
}

void assert(int beZero, char *msg) {
    if (beZero != 0) {
        printf("\n%s\n", msg);
        exit(beZero);
    }
}

void lockOnMode(int forMode) {
    if (forMode == mode) {
        assert(pthread_mutex_lock(&write_mutex), "Failed to lock mutex for writing! \n");
    }
}
void unlockOnMode(int forMode) {
    if (forMode == mode) {
        assert(pthread_mutex_unlock(&write_mutex), "Failed to unlock mutex for writing!\n");
    }
}

void updateCondOnMode(int forMode, long nextThread) {
    if (forMode == mode) {
        assert(pthread_mutex_lock(&cond_mutex), "Failed to lock mutex before broadcasting to pthread_condition.\n");
        currentThread = nextThread;
        assert(pthread_cond_broadcast(&cond), "Failed to broadcast to pthread condition.\n");
        assert(pthread_mutex_unlock(&cond_mutex), "Failed to unlock mutex after broadcasting to pthread_condition.\n");
    }
}
