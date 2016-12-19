#ifndef __ALG_DISK_CACHE_H__
#define __ALG_DISK_CACHE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct alg_disk_cache_s     alg_disk_cache_t;

struct alg_disk_cache_s {
    FILE    *file;
};

int alg_disk_cache_open(alg_disk_cache_t *dcache);
void alg_disk_cache_close(alg_disk_cache_t *dcache);

ssize_t alg_disk_cache_write(alg_disk_cache_t *dcache, const void *data, size_t size);
ssize_t alg_disk_cache_read(alg_disk_cache_t *dcache, void *data, size_t size);

size_t alg_disk_cache_peeksize(alg_disk_cache_t *dcache);

void alg_disk_cache_rollback(alg_disk_cache_t *dcache, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* __ALG_DISK_CACHE_H__ */
