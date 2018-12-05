#include "list.h"

typedef enum parseState {
    parseState_default = 0,
    parseState_escaped = 1,
    parseState_variable = 2,
    parseState_singleQuoted = 4,
    parseState_doubleQuoted = 8,
} parseState_t;


list_t *parse(char *input, char *envp[]);
unsigned isVariableValid(char isValid);
char *appendString(char *str1, unsigned strlen1, char *str2, unsigned strlen2);
char *getVariableValue(char *envp[], char name[]);
unsigned isState(unsigned is, parseState_t state);