#ifndef TP_HASH_FUNCIONES_HASH_H
#define TP_HASH_FUNCIONES_HASH_H
#include "stddef.h"
typedef size_t(*funcion_hash_t)(const char*, size_t);

size_t hash_01(const char *, size_t);
size_t hash_02(const char*, size_t);
size_t hash_03(const char*, size_t);

#endif //TP_HASH_FUNCIONES_HASH_H
