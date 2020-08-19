#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modules.h"

void init_modules(module_t *head)
{
    while (head)
    {
        if (head->enabled)
        {
            head->ctx = head->op.module_init();
        }
        head = head->next;
    }
}

void do_modules(module_t *head)
{
    while (head)
    {
        if (head->enabled)
        {
            if (head->op.module_do(head->ctx)) {
                head = head->next;
            }
        } else {            
            head = head->next;
        }
    }
}

void finish_modules(module_t *head)
{
    while (head)
    {
        if (head->enabled)
        {
            head->op.module_finish(head->ctx);
        }
        head = head->next;
    }
}