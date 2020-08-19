
#include "module_sqlite3.h"

// https://mangsby.com/blog/programming/c/c-c-sqlite-%EB%8D%B0%EC%9D%B4%ED%84%B0%EB%B2%A0%EC%9D%B4%EC%8A%A4-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0-1-%EA%B8%B0%EB%B3%B8-%EB%B2%88%EC%97%AD/


static void* module_init_sqlite3() {
    module_sqlite3_ctx_t *ctx = NULL;

    return ctx;
}

static bool module_do_sqlite3(void *ctx) {

    return true;
}

static bool module_finish_sqlite3(void *ctx) {
    
    return true;
}

module_op_t sqlite3 = {
    .module_init = module_init_sqlite3,
    .module_do = module_do_sqlite3,
    .module_finish = module_finish_sqlite3,
};