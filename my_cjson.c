#include "my_cjson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_json(json_object *item, char* string) {
    if(*string != '\"') {
        printf("Invalid input!\n");
        return ERR_INVALID_INPUT;
    }

    char* fast = string + 1, *slow = string;
    int count = 0;
    // Parse key
    while(*fast != '\"') {
        fast++;
        count++;
    }
    item->key = (char *)malloc((count+1)*sizeof(char));
    if(!(item->key)) {
        printf("malloc failed!\n");
        return ERR_MALLOC_FAILED;
    }
    memcpy(item->key, slow, count);

    fast++;
    while(*fast != '\"') fast++;
    slow = fast;
    count = 0;
    while(*fast != '\"') {
        fast++;
        count++;
    }
    item->value = (char *) malloc((count+1)*sizeof(char));
    if(!(item->value)) {
        printf("malloc failed!\n");
        return ERR_MALLOC_FAILED;
    }
    memcpy(item->value, slow, count);

}