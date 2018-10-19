#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char charToUpperCase(char input)
{
    if (input < 97 || input > 122)
    {
        return input;
    }
    return input - 32;
}

void toUpperCase(char *input)
{
    int length = strlen(input);
    for (int i = 0; i < length; i++)
    {
        input[i] = charToUpperCase(input[i]);
    }
}

void removeLineBreak(char *input)
{
    int length = strlen(input);
    int j = 0;
    for (int i = 0; i < length; i++)
    {
        if (input[i] != '\n' && input[i] != '\r')
        {
            input[j] = input[i];
            j++;
        }
        else
        {
            input[j] = '\0';
        }
    }
}

int main(int argc, char **argv)
{
    char input[128];
    fgets(input, 128, stdin);
    toUpperCase(input);
    removeLineBreak(input);
    printf("Hallo: %s -- VATB", input);
}
