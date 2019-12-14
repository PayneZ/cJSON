#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "mycJSON.h"

static const char *ep;

const char *cJSON_GetErrorPtr(void) {return ep;}

static int cJSON_strcasecmp(const char *s1, const char *s2);

static void *(*cJSON_malloc)(size_t sz) = malloc;
static void (*cJSON_free)(void *ptr) = free;

static char* cJSON_strdup(const char* str)
{
    size_t len;
    char *copy;

    len = strlen(str) + 1;
    copy = (char *) cJSON_malloc(len);
    if(!copy)
        return 0;
    memcpy(copy, str, len);
    return copy;
}

void cJSON_InitHooks(cJSON_Hooks* hooks)
{
    if(!hooks) {
        cJSON_malloc = malloc;
        cJSON_free = free;
        return;
    }

    cJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
    cJSON_free = (hooks->free_fn)?hooks->free_fn:free;
}

/* Internal constructor */
static cJSON *cJSON_New_Item(void)
{
    cJSON *node = (cJSON *) cJSON_malloc(sizeof(cJSON));
    if(node) memset(node, 0, sizeof(cJSON));
    return node;
}

static cJSON_Delete(cJSON *c)
{
    cJSON* next;
    while(c)
    {
        next = c->next;
        if(!(c->type&cJSON_IsReference) && c->child) cJSON_Delete(c->child);
        if(!(c->type&cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
        if(!(c->type&cJSON_StringIsConst) && c->string) cJSON_free(c->string);
        cJSON_free(c);
        c=next;
    }
}

static const char *parse_number(cJSON *item, const char *num){
    double n=0, sign=1, scale=0; int subscale=0, signsubscale=1;
    if(*num=='-') sign=-1, num++;
    if(*num=='0') num++;
    if(*num>='1' && *num <='9'){
        do
        {
            n = n*10 + (*num - '0');
            num++;
        } while (*num>='0' && *num<='9');
    }
    if(*num=='.' && num[1] >='0' && num[1] <= '9') {
        do
        {
            n = n*10 + (*num-'0');
            scale--;
            num++;
        } while (num[1] >='0' && num[1] <= '9');
    }
    if(*num=='e' || *num=='E') {
        num++;
        if(*num=='+') num++;
        else if(*num=='-') num++, signsubscale=-1;
        while(*num>='0' && *num<='9') {
            subscale = subscale*10 + *num-'0';
            num++;
        }
    }

    n = sign*n*pow(10.0, (scale+subscale*signsubscale));

    item->valuedouble = n;
    item->valueint=(int)n;
    item->type=cJSON_Number;
    return num;
}

static int pow2gt(int x);

typedef struct {
    char* buffer;
    int length;
    int offset;
} printbuffer;

static char* ensure(printbuffer *p, int needed);

static int update(printbuffer *p);

static char *print_number(cJSON *item, printbuffer *p);

static unsigned parse_hex4(const char *str);

static const char *parse_string(cJSON *item, const char *str);