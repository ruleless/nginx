#include "alg_disk_cache.h"

int
alg_disk_cache_open(alg_disk_cache_t *dcache)
{
    dcache->file = tmpfile();
    
    return dcache->file != NULL;
}

void
alg_disk_cache_close(alg_disk_cache_t *dcache)
{
    if (dcache->file) {
        fclose(dcache->file);
    }
}

ssize_t
alg_disk_cache_write(alg_disk_cache_t *dcache, const void *data, size_t size)
{
    long curpos;
    
    if (NULL == dcache->file &&
        !alg_disk_cache_open(dcache)) {
        return -1;
    }

    curpos = ftell(dcache->file);
    if (curpos < 0) {
        return -10;
    }
    if (fseek(dcache->file, 0, SEEK_END)) {
        return -11;
    }

    if (fwrite(&size, 1, sizeof(size), dcache->file) != sizeof(size)) {
		return -2;
	}
	if (fwrite(data, 1, size, dcache->file) != size) {
		return -3;
	}

	fseek(dcache->file, curpos, SEEK_SET);

	return size;
}

ssize_t
alg_disk_cache_read(alg_disk_cache_t *dcache, void *data, size_t size)
{
	size_t peeksz;
	
	if (NULL == dcache->file) {
		return 0;
	}

	peeksz = alg_disk_cache_peeksize(dcache);
	if (0 == peeksz) {
		return 0;
	}
	if (size < peeksz) {
		return -2;
	}

	fseek(dcache->file, sizeof(peeksz), SEEK_CUR);
	return fread(data, 1, peeksz, dcache->file);
}

size_t
alg_disk_cache_peeksize(alg_disk_cache_t *dcache)
{
	size_t peeksz;
	
	if (NULL == dcache->file) {
		return 0;
	}

	peeksz = 0;
	if (fread(&peeksz, 1, sizeof(peeksz), dcache->file) != sizeof(peeksz)) {
		return 0;
	}

	fseek(dcache->file, -sizeof(peeksz), SEEK_CUR);
	return peeksz;
}

void
alg_disk_cache_rollback(alg_disk_cache_t *dcache, size_t n)
{
	if (dcache->file) {
		fseek(dcache->file, -(n+sizeof(size_t)), SEEK_CUR);
	}
}
