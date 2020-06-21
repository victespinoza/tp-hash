
#include <stddef.h>
#include "lista.h"

typedef void (*hash_destruir_dato_t)(void*);
typedef struct {
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
}tabla_hash_t;
typedef struct {
    void* clave;
    void* valor;
}clave_valor_t;

