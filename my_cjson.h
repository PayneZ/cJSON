typedef enum ERR_CODES{
    ERR_SUCCESS,
    ERR_MALLOC_FAILED,
    ERR_INVALID_INPUT
} ERR_CODES;

//void print_error_message(ERR_CODES code) {
//    switch(code) {
//        case ERR_SUCCESS:
//            printf("Succeeded!\n");
//            break;
//        case ERR_MALLOC_FAILED:
//            printf("Failed to allocate memory!\n");
//            break;
//        case ERR_INVALID_INPUT:
//            printf("Invalid Input!\n");
//            break;
//        default:
//            printf("Invalid Error Code!\n");
//    }
//}

typedef enum JSON_TYPES {
    JSON_TYPE_JSON,
    JSON_TYPE_STRING
} JSON_TYPES;

struct _json_object {
    char *key;
    void* value;
    struct _json_object *next;
    JSON_TYPES type;
};

typedef struct _json_object json_object;

void print_simple_json(json_object *json);

ERR_CODES parse_json(json_object *json_p, char* string);