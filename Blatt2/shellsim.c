#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE !FALSE
#define EXITCOMAND "exit\0"
#define EXITCOMANDLENGTH 4

int main(int argc, char **argv)
{
    char input[265];
    while (TRUE)
    {
        fgets(input, 265, stdin);
        if (strncmp(input, EXITCOMAND, EXITCOMANDLENGTH) == 0)
        {
            break;
        }
        system(input);
    }
    return 0;
}
