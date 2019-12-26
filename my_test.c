#include <stdio.h>
#include <stdlib.h>
#include "my_cjson.h"

void print_test_seperation(int num){
    printf("====testing case %2d====\n", num);
}

int main() {
//	char text1[]="{n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";	
//	char text2[]="{n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";	
//    printf("%s\n", text0);

    // case 1: test print_simple_json
    print_test_seperation(1);
    char* key = "key";
    char* value = "value";
    json_object *json_p = malloc(sizeof(json_object));
    json_p->key = key;
    json_p->value = value;
    json_p->type = JSON_TYPE_STRING;
    print_simple_json(json_p);

    // case 2: test parse_json
    print_test_seperation(2);
    char case2[] = "{\"key\": \"value\"}"; 
    json_object *json_p2 = malloc(sizeof(json_object));
    parse_json(json_p2, case2);
    print_simple_json(json_p2);
    

    return 0;
}