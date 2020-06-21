#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"

#define TAMANIO 97;
typedef struct {
    void* clave;
    void* valor;
}clave_valor_t;
struct hash{
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
};
typedef struct {
    hash_t* hash;
    lista_t* actual;
}hash_iter;

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->tamanio = TAMANIO;
    hash->cantidad = 0;
    hash->tabla = lista_crear();
    if (hash->tabla == NULL){
        free(hash);
        return NULL;
    }
    return hash;
}