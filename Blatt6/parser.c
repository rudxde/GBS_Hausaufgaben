#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "list.h"
#include "parser.h"

#define FALSE 0
#define TRUE !FALSE

list_t *parse(char *input, char *envp[]) {
    list_t *commandList = list_init();
    unsigned state = parseState_default;
    char *stringInProgress = malloc(sizeof(char) * 1024);
    char *variableNameInProgress = malloc(sizeof(char) * 1024);
    int stringInProgressPointer = 0;
    int variableInProgressPointer = 0;
    for (unsigned inputProgressPointer = 0; input[inputProgressPointer] != '\0'; inputProgressPointer++) {
        switch (input[inputProgressPointer]) {
            case '\n':
                break;
            case '"':
                // printf(">\" state: %u\n", state);
                if (isState(state, parseState_default)) {
                    state |= parseState_doubleQuoted;
                } else if (isState(state, parseState_escaped)) {
                    state ^= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_variable)) {
                    state &= ~parseState_variable;
                    char *variableValue = getVariableValue(envp, variableNameInProgress);
                    unsigned variablenamelength = strlen(variableValue);
                    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                    stringInProgressPointer += variablenamelength;
                    variableInProgressPointer = 0;
                    variableNameInProgress = malloc(sizeof(char) * 1024);
                    if (isState(state, parseState_doubleQuoted)) {
                        state &= ~parseState_doubleQuoted;
                    }
                } else if (isState(state, parseState_doubleQuoted)) {
                    state &= ~parseState_doubleQuoted;
                    printf("A");
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case '\'':
                if (isState(state, parseState_default)) {
                    state |= parseState_singleQuoted;
                } else if (isState(state, parseState_escaped)) {
                    state ^= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_variable)) {
                    state &= ~parseState_variable;
                    char *variableValue = getVariableValue(envp, variableNameInProgress);
                    unsigned variablenamelength = strlen(variableValue);
                    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                    stringInProgressPointer += variablenamelength;
                    variableInProgressPointer = 0;
                    variableNameInProgress = malloc(sizeof(char) * 1024);
                    if (isState(state, parseState_singleQuoted)) {
                        state &= ~parseState_singleQuoted;
                    }
                } else if (isState(state, parseState_singleQuoted)) {
                    state &= ~parseState_singleQuoted;
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            case '$':
                if (isState(state, parseState_escaped)) {
                    state ^= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_variable)) {
                    state &= ~parseState_variable;
                    char *variableValue = getVariableValue(envp, variableNameInProgress);
                    unsigned variablenamelength = strlen(variableValue);
                    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                    stringInProgressPointer += variablenamelength;
                    variableInProgressPointer = 0;
                    variableNameInProgress = malloc(sizeof(char) * 1024);
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else {
                    state |= parseState_variable;
                }
                break;
            case '\\':
                if (isState(state, parseState_escaped)) {
                    state &= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_variable)) {
                    state &= ~parseState_variable;
                    char *variableValue = getVariableValue(envp, variableNameInProgress);
                    unsigned variablenamelength = strlen(variableValue);
                    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                    stringInProgressPointer += variablenamelength;
                    variableInProgressPointer = 0;
                    variableNameInProgress = malloc(sizeof(char) * 1024);
                    state |= parseState_escaped;
                } else {
                    state |= parseState_escaped;
                }
                break;
            case ' ':
                if (isState(state, parseState_default)) {
                    stringInProgress[stringInProgressPointer++] = '\0';
                    list_append(commandList, stringInProgress);
                    stringInProgress = malloc(sizeof(char) * 1024);
                    stringInProgressPointer = 0;
                } else if (isState(state, parseState_variable)) {
                    state &= ~parseState_variable;
                    char *variableValue = getVariableValue(envp, variableNameInProgress);
                    unsigned variablenamelength = strlen(variableValue);
                    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                    stringInProgressPointer += variablenamelength;
                    variableInProgressPointer = 0;
                    variableNameInProgress = malloc(sizeof(char) * 1024);
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_escaped)) {
                    state &= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
            default:
                if (isState(state, parseState_escaped)) {
                    state &= ~parseState_escaped;
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                } else if (isState(state, parseState_variable)) {
                    if (isVariableValid(input[inputProgressPointer])) {
                        variableNameInProgress[variableInProgressPointer++] = input[inputProgressPointer];
                    } else {
                        state &= ~parseState_variable;
                        char *variableValue = getVariableValue(envp, variableNameInProgress);
                        unsigned variablenamelength = strlen(variableValue);
                        stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
                        stringInProgressPointer += variablenamelength;
                        variableInProgressPointer = 0;
                        variableNameInProgress = malloc(sizeof(char) * 1024);
                        stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                    }
                } else {
                    stringInProgress[stringInProgressPointer++] = input[inputProgressPointer];
                }
                break;
        }
    }
    char *variableValue = getVariableValue(envp, variableNameInProgress);
    unsigned variablenamelength = strlen(variableValue);
    stringInProgress = appendString(stringInProgress, stringInProgressPointer, variableValue, variablenamelength);
    stringInProgressPointer += variablenamelength;
    stringInProgress[stringInProgressPointer++] = '\0';
    list_append(commandList, stringInProgress);
    return commandList;
}

unsigned isVariableValid(char isValid) {
    if (isValid >= 'A' && isValid <= 'Z') return TRUE;
    if (isValid >= '0' && isValid <= '9') return TRUE;
    if (isValid == '_') return TRUE;
    return FALSE;
}

char *appendString(char *str1, unsigned strlen1, char *str2, unsigned strlen2) {
    char *result = malloc(sizeof(char) * 1024);
    for (unsigned i = 0; i < strlen1; i++) {
        result[i] = str1[i];
    }
    for (unsigned i = strlen1; i < strlen1 + strlen2; i++) {
        result[i] = str2[i - strlen1];
    }
    return result;
}

char *getVariableValue(char *envp[], char name[]) {
    for (int i = 0; envp[i] != NULL; i++) {
        size_t arglen = strlen(envp[i]);
        char *varname = malloc(sizeof(char) * arglen);
        int j = 0;
        char current = envp[i][j];
        while ('=' != current) {
            varname[j] = current;
            j++;
            current = envp[i][j];
        }
        varname[j] = '\0';
        j++;

        if (0 == strcmp(varname, name)) {                 // we found the evironment variable with the right name
            char *value = malloc(arglen * sizeof(char));  // TODO: make sure this is freed up at some point (should be fine though)
            int k = 0;
            current = envp[i][j];
            while ('\0' != current) {
                value[k] = current;
                k++;
                j++;
                current = envp[i][j];
            }
            value[k] = '\0';
            free(varname);
            return value;
        }
        free(varname);
    }
    // no environment variable with the right name exists
    char *emptyStr = malloc(sizeof(char));
    emptyStr[0] = '\0';
    return emptyStr;
}

unsigned isState(unsigned is, parseState_t state) {
    if (state == parseState_default) return is == state;
    return (is & state) == state;
}
