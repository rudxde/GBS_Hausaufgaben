#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0;
#define TRUE !FALSE;

char getPrintAble(char input);

void hexdump (FILE *output, char *buffer, int length) {
    short bufferEnded = FALSE;
    int pufferOffset = 0;
    char lineString[17];
    lineString[16] = '\0';
    while(!bufferEnded) {
        int lineStart = pufferOffset;

        fprintf(output, "%06x : ", lineStart);
        int lineLength = 0;
        for(int i = 0; i < 16; i++) {
            if(pufferOffset <= length) {
                fprintf(output, "%02x ", buffer[lineStart + i] & 0xFF);
                lineString[i] = getPrintAble(buffer[lineStart + i]);
                pufferOffset++;
                lineLength++;
            } else {
                bufferEnded = TRUE;
                lineString[i] = '\0';
                fprintf(output, "   ");
            }
        }
        fprintf(output, "  %s\n", lineString);
        if(pufferOffset > length) {
            bufferEnded = TRUE;
        }
    }

}

char getPrintAble(char input) {
    if (input < 0x20 || input > 0x7e) {
        return '.';
    } else {
        return input;
    }
}
