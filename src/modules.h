
#ifndef _MODULES_H_
#define _MODULES_H_


#include <stdbool.h>

typedef struct module_op_t {
    void* (*module_init)();
    bool (*module_do)(void*);
    void (*module_finish)(void*);
} module_op_t;

typedef struct module_t {
    bool enabled;
    char name[64];
    module_op_t op;
    void* ctx;
    struct module_t* next;
} module_t;


void init_modules(module_t* head);
void do_modules(module_t* head);
void finish_modules(module_t* head);

#endif