#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0;
#define TRUE !FALSE;

char getPrintAble(char input);
void printLine(FILE *output, char *buffer, int length, int *pufferOffset, short *bufferEnded);

void hexdump (FILE *output, char *buffer, int length) {
    short bufferEnded = FALSE;
    int pufferOffset = 0;
    while(!bufferEnded) {
        printLine(output, buffer, length, &pufferOffset, &bufferEnded);
    }
}

/*
 * printLine
 * Prints one line of the hexdump, increases the pufferOffset counter and sets the bufferEnded flag to TRUE if the buffer is processed completely.
 * @param {File *} output - on which output the hexdump should be printed.
 * @param {char *} buffer - the text, which a hexdump should be created from
 * @param {int} length - the length of th buffer
 * @param {int *} pufferOffset - a counter, how far the puffer is already printed
 * @param {short} bufferEnded - flag if the printing of the buffer is complete
 * @return {void} - --
 **/
void printLine(FILE *output, char *buffer, int length, int *pufferOffset, short *bufferEnded) {
    int lineStart = *pufferOffset;

    char lineString[17]; // The lineString is the second part of the hexdump, where the buffer is printed as readable chars.
    lineString[16] = '\0';

    // prints the bufferOffset at the line start
    fprintf(output, "%06x : ", lineStart);

    for(int i = 0; i < 16; i++) {
        // checks if the buffer has ended during the line
        if(*pufferOffset <= length) {
            fprintf(output, "%02x ", buffer[lineStart + i] & 0xFF);
            lineString[i] = getPrintAble(buffer[lineStart + i]);
            (*pufferOffset)++;
        } else {
            *bufferEnded = TRUE;
            lineString[i] = '\0';
            fprintf(output, "   ");
        }
    }

    // prints the line string at the end of the Line
    fprintf(output, "  %s\n", lineString);

    //checks if the buffer has ended directory at the end of the line.
    if(*pufferOffset > length) {
        *bufferEnded = TRUE;
    }
    free(lineString);
}

/*
 * getPrintAble
 * Returns a char which can be printed in the text area of the hexdump.
 * @param {char} input - the char, which will be used.
 * @return {char} - returns the input char if it is a char, that can be displayed. Else it returns a dot.
 **/
char getPrintAble(char input) {
    if (input < 0x20 || input > 0x7e) {
        return '.';
    } else {
        return input;
    }
}
