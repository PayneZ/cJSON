#include "my_cjson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* LEFT = "{\n";
static char* RIGHT ="\n}\n";
static char* QUOTE = "\"";
static char* TAB = "\t";

void print_error_message(ERR_CODES code) {
    switch(code) {
        case ERR_SUCCESS:
            printf("Succeeded!\n");
            break;
        case ERR_MALLOC_FAILED:
            printf("Failed to allocate memory!\n");
            break;
        case ERR_INVALID_INPUT:
            printf("Invalid Input!\n");
            break;
        default:
            printf("Invalid Error Code!\n");
    }
}

void print_simple_json(json_object *json){
    char* result;
    char* key = json->key;
    char* value = json->value;
    int len = strlen(key) + strlen(value) + strlen(LEFT) + strlen(RIGHT); 
    result = malloc(1);
    printf("%d\n", sizeof(char));

    int i = 0;
    for(i = 0; i < len+3; i++) result[i] = '0';
    result[0] = '\0';

    sprintf(result, "%s\"%s\": \"%s\"%s", LEFT, key, value, RIGHT);

    printf(result);
}

/* allocate memory for *key_p, then fill it with characters from start to end.
*/
ERR_CODES malloc_and_fill(char** key_p, char* start, char *end) {
    int size = end-start;
    *key_p = malloc(sizeof(char) * size);
    if(!(*key_p)) {
        //print_error_message(ERR_MALLOC_FAILED);
        return ERR_MALLOC_FAILED;
    }
    memcpy(*key_p, start, size);
    return ERR_SUCCESS;
}

ERR_CODES parse_json(json_object *item, char* string) {
    ERR_CODES code = ERR_SUCCESS;
    if(string == NULL) {
        code = ERR_INVALID_INPUT;
        //print_error_message(code);
        return code;
    }

    // move to first left bracket. {
    while(*string != '{' && *string != '\0') string++;
    if(*string == '\0') {
        code = ERR_INVALID_INPUT;
        //print_error_message(code);
        return code;
    }

    char* fast = string, *slow = string;
    // Parse key
    //// move to first quote
    while(*fast != *QUOTE) fast++;
    slow = ++fast;
    while(*fast != *QUOTE) fast++;
    malloc_and_fill(&(item->key), slow, fast);

    // Parse value
    //// move to fourth quote
    fast++;
    while(*fast != *QUOTE) fast++;
    slow = ++fast;
    while(*fast != *QUOTE) fast++;
    malloc_and_fill(&(item->value), slow, fast);

    return code;
}