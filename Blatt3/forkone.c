#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]);
void parent(int pid);
void child(int K);
void printDate(char prefix[]);


int main(int argc, char *argv[], char *envp[])
{
    printDate("Start:");
    if(argc < 2) {
        printf("needs parameter K");
        exit(-1);
    }
    int K = atoi(argv[1]);
    int pid = fork();
    if (pid == -1)
    {
        printf("childprocess could not be created!");
        exit(-1);
    }
    else if (pid == 0)
    {
        // Im the child
        child(K);
    }
    else
    {
        // Im the parent
        parent(pid);
    }
}

void parent(int pid)
{
    int exitCode = 0;
    wait(&exitCode);
    printf("Exit-Code: %i\n", WEXITSTATUS(exitCode));
    printDate("End:");
    exit(0);
}

void child(int K)
{
    int myPid = getpid();
    int parentPid = getppid();
    for (unsigned i = 1; i <= 10; i++)
    {
        sleep(1);
        printf("%6i %6i %u\n", myPid, parentPid, i);
    }
    exit((myPid + K) % 100);
}

void printDate(char prefix[])
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // TODO: adapt date format
    printf("%s %d-%d-%d %d:%d:%d\n", prefix, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}