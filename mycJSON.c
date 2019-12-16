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

static int pow2gt(int x) {
    /*
    given x, find the smallest y of the form 2^n, such that y >= x;
    */
    --x;       //x=1000 1111;
    x |= x>>1; //x>>1 = 0100 0111; x=1100 1111;
    x |= x>>2; //x>>2 = 0011 0011; x=1111 1111;
    x |= x>>4; //x>>4 = 0000 1000; x=1110 1111
    x |= x>>8;
    x |= x>>16;
    return x+1;
}

typedef struct {
    char* buffer;
    int length;
    int offset;
} printbuffer;

static char* ensure(printbuffer *p, int needed)
{
    if(!p || !p->buffer) return 0;
    needed += p->offset;
    if(needed <= p->length) return p->buffer + p->offset;

    int newsize = pow2gt(needed);
    char* newbuffer = (char *)cJSON_malloc(newsize);
    if(!newbuffer) {
        cJSON_free(p->buffer);
        p->length = 0, p->offset = 0;
        return 0;
    }
    //else
    memcpy(newbuffer, p->buffer, p->length);
    cJSON_free(p->buffer);
    p->buffer = newbuffer;
    p->length = newsize;
    return p->buffer + p->offset;
}

static int update(printbuffer *p) {
    char *str;
    if(!p || !p->buffer) return 0;
    str = p->buffer + p->offset;
    return p->offset + strlen(str);
}

static char *print_number(cJSON *item, printbuffer *p) 
{
    char *str = 0;
    double d=item->valuedouble;
    if(d == 0) 
    {
        if(p) str=ensure(p, 2);
        else str(char*)cJSON_malloc(2);
        if(str) strcpy(str, "0");
    } 
    else if (fabs(((double)item->valueint)-d)<= DBL_EPSILON && d<=INT_MAX && d>=INT_MIN) 
    {
        if(p) str = ensure(p, 21);
        else str=(char*)cJSON_malloc(21); /* 2^64 + 1 can be represented in 21 chars. */
        if(str) sprintf(str, "%d", item->valueint);
    } 
    else 
    {
        if(p) str = ensure(p, 64);
        else str = (char*)cJSON_malloc(64);
        if(str) 
        {
            if(fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60) sprinf(str, "%.0f", d);
            else if(fabs(d)<1.0e-6 || fabs(d)>1.0e9) sprintf(str, "%e", d);
            else sprintf(str, "%f", d);
        }
    }
    return str;
}

static unsigned parse_hex4(const char *str)
{
    unsigned h=0;
    if(*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++
    if(*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++
    if(*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++
    if(*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    return h;
}

static const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static const char *parse_string(cJSON *item, const char *str);