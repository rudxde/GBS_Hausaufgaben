#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define FALSE 0
#define TRUE !FALSE

typedef enum { commandtype_string, commandtype_var } commandType_t;

typedef struct command {
    commandType_t type;
    char *variableSelector;
    char *value;
} command_t;

int main(int argc, char **argv, char *envp[]) {
    inputLoop(envp);
    return 0;
}

void inputLoop(char *envp[]) {
    char input[1024];
    while (TRUE) {
        fgets(input, 265, stdin);
        parse(input, envp);
        // TODO
    }
}

typedef enum parseState {
    parseState_default,
    parseState_escaped,
    parseState_variable,
    parseState_singleQuoted,
    parseState_doubleQuoted,
    parseState_escaped_default,
    parseState_escaped_doubleQuoted,
    parseState_escaped_singleQuoted,
} parseState_t;

list_t *parse(char *input, char *envp[]) {
    list_t *commandList = list_init();
    parseState_t state = parseState_default;
    char *stringInProgress = malloc(sizeof(char) * 1024);
    int stringInProgressPointer = 0;
    for (unsigned inputProgressPointer = 0; input[inputProgressPointer] != '\0'; inputProgressPointer++) {
        switch (input[inputProgressPointer]) {
            case '"':
                if (state == parseState_default)
                    state = parseState_doubleQuoted;
                else if (state == parseState_doubleQuoted)
                    state = parseState_default;
                else if (state == parseState_variable)
                    throwWongVariableFormat();
                else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case '\'':
                if (state == parseState_default)
                    state = parseState_singleQuoted;
                else if (state == parseState_singleQuoted)
                    state = parseState_default;
                else if (state == parseState_variable)
                    throwWongVariableFormat();
                else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case '$':
                if (state == parseState_default)
                    state = parseState_variable;
                else if (state == parseState_variable)
                    throwWongVariableFormat();
                else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case '\\':
                if (state == parseState_default)
                    state = parseState_escaped_default;
                else if (state == parseState_doubleQuoted)
                    state = parseState_escaped_doubleQuoted;
                else if (state == parseState_singleQuoted)
                    state = parseState_escaped_singleQuoted;
                else if (state == parseState_variable)
                    throwWongVariableFormat();
                else if (state == parseState_escaped_default) {
                    state = parseState_default;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_doubleQuoted) {
                    state = parseState_doubleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_singleQuoted) {
                    state = parseState_singleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case ' ':
                if (state == parseState_default) {
                    stringInProgress[stringInProgressPointer++] = '\0';
                    command_t *newListCommand = malloc(sizeof(command_t));
                    newListCommand->type = commandtype_string;
                    newListCommand->value = stringInProgress;
                    newListCommand->variableSelector = NULL;
                    list_append(commandList, newListCommand);
                    stringInProgress = malloc(sizeof(char) * 1024);
                    stringInProgressPointer = 0;
                } else if (state == parseState_variable) {
                    stringInProgress[stringInProgressPointer++] = '\0';
                    command_t *newListCommand = malloc(sizeof(command_t));
                    newListCommand->type = commandtype_var;
                    newListCommand->variableSelector = stringInProgress;
                    newListCommand->value = getVariableValue(envp, stringInProgress);
                    list_append(commandList, newListCommand);
                    stringInProgress = malloc(sizeof(char) * 1024);
                    stringInProgressPointer = 0;
                } else if (state == parseState_escaped_default) {
                    state = parseState_default;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_doubleQuoted) {
                    state = parseState_doubleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_singleQuoted) {
                    state = parseState_singleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            default:
                if (state == parseState_escaped_default) {
                    state = parseState_default;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_doubleQuoted) {
                    state = parseState_doubleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (state == parseState_escaped_singleQuoted) {
                    state = parseState_singleQuoted;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
        }
    }
    return commandList;
}

void throwWongVariableFormat() {
    printf("");
    exit(-1);
}

char *getVariableValue(char *envp[], char name[]) {
    // TODO
}
