#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char charToUpperCase(char input);
char * stringToUpperCase(const char *input, unsigned int length);
void removeLineBreak(char *input, unsigned int length);
char * rotup(char *input, unsigned int length);

int main(int argc, char **argv)
{
    char input[128];
    unsigned int inputLen = read(0, input, 128);
    removeLineBreak(input, inputLen);
    char * upperInput = stringToUpperCase(input, inputLen);
    char * rotInput = rotup(upperInput, inputLen);

    unsigned int outputLen = inputLen * 2 + 13; 
    char * output = malloc(outputLen); 
    sprintf(output, "Hallo: %s -- %s\n", input, rotInput);
    write(1, output, outputLen);

    free(upperInput);
    free(rotInput);
    free(output);
    return 0;
}



char charToUpperCase(char input)
{
    if (input < 97 || input > 122)
    {
        return input;
    }
    return input - 32;
}

char * stringToUpperCase(const char *input, unsigned int length)
{
    char *result = malloc(length);
    for (unsigned int i = 0; i < length; i++)
    {
        result[i] = charToUpperCase(input[i]);
    }
    return result;
}

void removeLineBreak(char *input, unsigned int length)
{
    int j = 0;
    for (unsigned int i = 0; i < length; i++)
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

char * rotup(char *input, unsigned int length)
{
    char * output = malloc(length);
    for (unsigned int i = 0; i < length; i++)
    {
        if(input[i] < 0x41 || input[i] > 0x5A) {
            output[i] = '\0';
            return output;
        }
        output[i] = (char)((((input[i] - 0x41) + 13) % 26) + 0x41);
    }
    return output;
}