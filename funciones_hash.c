#include "funciones_hash.h"
#include "string.h"
#include <stdint.h>
#define FNV_PRIME_64 1099511628211U
#define FNV_OFFSET_64 14695981039346656037U
size_t hash_01(const char *str, size_t tamanio) {
    unsigned long hash = 5381;
    int c = *str;
    while (c) {
        hash = ((hash << 5) + hash) + c;
        c = *str++;
    }
    return hash % tamanio;
}

size_t hash_02(const char *clave, size_t largo_tabla_hash) {
    //Jenkins Hashing
    //Ref: https://en.wikipedia.org/wiki/Jenkins_hash_function
    size_t i = 0, n = strlen((char*)clave);
    uint32_t hash = 0;
    while (i != n) {
        hash += ((unsigned char*)clave)[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return (size_t)(hash%largo_tabla_hash);
}

size_t hash_03(const char* clave, size_t largo_tabla_hash) {
    // murmur hash, ref: http://bitsquid.blogspot.com/2011/08/code-snippet-murmur-hash-inverse-pre.html
    const uint_fast64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint_fast64_t h = FNV_PRIME_64 ^ (largo_tabla_hash * m);

    const uint_fast64_t * data = (const uint_fast64_t *)clave;
    const uint_fast64_t * end = data + (largo_tabla_hash/8);

    while(data != end)
    {
#ifdef PLATFORM_BIG_ENDIAN
        uint_fast64_t k = *data++;
			char *p = (char *)&k;
			char c;
			c = p[0]; p[0] = p[7]; p[7] = c;
			c = p[1]; p[1] = p[6]; p[6] = c;
			c = p[2]; p[2] = p[5]; p[5] = c;
			c = p[3]; p[3] = p[4]; p[4] = c;
#else
        uint_fast64_t k = *data++;
#endif

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(largo_tabla_hash & 7) {
        case 7:
            h ^= (uint_fast64_t)data2[6] << 48;
            break;
        case 6:
            h ^= (uint_fast64_t)data2[5] << 40;
            break;
        case 5:
            h ^= (uint_fast64_t)data2[4] << 32;
            break;
        case 4:
            h ^= (uint_fast64_t)data2[3] << 24;
            break;
        case 3:
            h ^= (uint_fast64_t)data2[2] << 16;
            break;
        case 2:
            h ^= (uint_fast64_t)data2[1] << 8;
            break;
        case 1:
            h ^= (uint_fast64_t)data2[0];
            h *= m;
            break;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return (int)(h%largo_tabla_hash);
}

