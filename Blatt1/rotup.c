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

char * toUpperCase(const char *input)
{
    int length = strlen(input);
    char *result = malloc(length);
    for (int i = 0; i < length; i++)
    {
        result[i] = charToUpperCase(input[i]);
    }
    return result;
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

char * rotup(char *input, int length)
{
    char * output = malloc(length);
    for (int i = 0; i < length; i++)
    {
        if(input[i] < 0x41 || input[i] > 0x5A) {
            output[i] = '\0';
            return output;
        }
        output[i] = (char)((((input[i] - 0x41) + 13) % 26) + 0x41);
    }
    return output;
}

int main(int argc, char **argv)
{
    char input[128];
    fgets(input, 128, stdin);
    removeLineBreak(input);
    char *upperInput = toUpperCase(input);
    printf("Hallo: %s -- ", input);
    char * rotInput = rotup(upperInput, 128);
    printf("%s\n", rotInput);
    free(upperInput);
    free(rotInput);
    return 0;
}
