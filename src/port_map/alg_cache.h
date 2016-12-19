#ifndef __ALG_CACHE_H__
#define __ALG_CACHE_H__

#include "list.h"
#include "alg_disk_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct alg_cache_seg_s  alg_cache_seg_t;
typedef struct alg_cache_s      alg_cache_t;

typedef int (*alg_cache_flush_handler_pt)(alg_cache_t *t, const void *data, size_t size);

struct alg_cache_seg_s {
    struct list_head    node;
    
    size_t              size;
    char               *data;
};

struct alg_cache_s {
    struct list_head            memcache;
    alg_disk_cache_t            dcache;

    void                       *data;

    size_t                      memcache_size;
    size_t                      dcache_size;

    size_t                      memcache_maxsize;

    alg_cache_flush_handler_pt  handler;

    void                     *(*alloc)(void *alloc_ctx, size_t size);
    void                      (*dealloc)(void *alloc_ctx, void *m);
    void                       *alloc_ctx;
};

alg_cache_t *alg_cache_create(alg_cache_flush_handler_pt h,
                              void *(*alloc)(void *alloc_ctx, size_t size),
                              void (*dealloc)(void *alloc_ctx, void *m),
                              void *alloc_ctx);

void alg_cache_destroy(alg_cache_t *cache);

int alg_cache_push(alg_cache_t *cache, const void *data, size_t size);
int alg_cache_flushall(alg_cache_t *cache);

int alg_cache_empty(alg_cache_t *cache);

#ifdef __cplusplus
}
#endif

#endif /* __ALG_CACHE_H__ */
