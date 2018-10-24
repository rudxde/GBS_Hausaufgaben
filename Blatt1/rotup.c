#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char charToUpperCase(char input);
char * stringToUpperCase(const char *input, unsigned length);
void removeLineBreak(char *input, unsigned length);
char * rotStringUp(char *input, unsigned length);

int main(int argc, char **argv)
{
    char input[128];
    unsigned inputLength = read(0, input, 128);
    removeLineBreak(input, inputLength);
    char * upperCaseInput = stringToUpperCase(input, inputLength);
    char * inputRotUp = rotStringUp(upperCaseInput, inputLength);

    unsigned outputLen = inputLength * 2 + 13; 
    char * output = malloc(outputLen); 
    sprintf(output, "Hallo: %s -- %s\n", input, inputRotUp);
    write(1, output, outputLen);

    free(upperCaseInput);
    free(inputRotUp);
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

char * stringToUpperCase(const char *input, unsigned length)
{
    char *result = malloc(length);
    for (unsigned i = 0; i < length; i++)
    {
        result[i] = charToUpperCase(input[i]);
    }
    return result;
}

void removeLineBreak(char *input, unsigned length)
{
    int j = 0;
    for (unsigned i = 0; i < length; i++)
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

char * rotStringUp(char *input, unsigned length)
{
    char * output = malloc(length);
    for (unsigned i = 0; i < length; i++)
    {
        // break if char is not [A-Z]
        if(input[i] < 0x41 || input[i] > 0x5A) {
            output[i] = '\0';
            return output;
        }
        output[i] = (char)((((input[i] - 0x41) + 13) % 26) + 0x41);
    }
    return output;
}
