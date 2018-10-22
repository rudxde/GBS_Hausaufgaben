#include <stdio.h>

int main (void) {
    unsigned int i = 0;
    printf("Oct\tDec\tHex\tChar\n");
    for(i = 0; i < 128; i++){
        printf("%03o\t%-3u\t%02x\t%c\n",i,i,i,(char) i);
    }
}
