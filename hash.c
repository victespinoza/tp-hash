#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"
#include <string.h>
#include "funciones_hash.h"
#include "utils.h"

const size_t TAMANIO = 97;
#define CAPACIDAD_BUCKETS 4
const double FACTOR_CARGA = 0.96;
const size_t CANTIDAD_FUNCIONES_HASH = 2;
funcion_hash_t funciones_hash[3] = {hash_01, hash_02, hash_03};

typedef struct {
    char* clave;
    void* valor;
    size_t cantidad_hash_aplicado;
} clave_valor_t;
struct hash {
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
};
struct hash_iter {
    const hash_t* hash;
    size_t posicion_tabla;
    size_t recorridos;
    lista_iter_t* iter_bucket_actual;
};

hash_t* _hash_crear(hash_destruir_dato_t destruir_dato, size_t capacidad);
bool _hash_guardar(hash_t *hash, clave_valor_t* claveValor);
hash_t* _reasignar_posiciones_hash(hash_t* hash, size_t capacidad);
clave_valor_t* _buscar(const char* clave, const hash_t* hash);
clave_valor_t* _crear_clave_valor(const char* clave, void* dato);
void _recorrer_buckets(hash_t* hash,
                       bool visitar(lista_iter_t* iter_bucket, void* dato, hash_destruir_dato_t destruir_dato),
                       void* datos, hash_destruir_dato_t destruir_dato);
bool _destruir_bucket(lista_iter_t* iter_bucket, void* dato, hash_destruir_dato_t destruir_dato);
bool _guardar_nuevo_hash(lista_iter_t* iter_bucket, void* nuevo_hash, hash_destruir_dato_t destruir_dato);

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
    return _hash_crear(destruir_dato, TAMANIO);
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    _recorrer_buckets(hash, _destruir_bucket, NULL, hash->destruir_dato);
    free(hash->tabla);
    free(hash);
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
    clave_valor_t* actual = _buscar(clave, hash);
    return actual != NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    clave_valor_t* actual = _buscar(clave, hash);
    if (actual == NULL){
        return NULL;
    }
    return actual->valor;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    clave_valor_t* clave_valor = _buscar(clave, hash);
    if (clave_valor != NULL){
        if(hash->destruir_dato != NULL){
            hash->destruir_dato(clave_valor->valor);
        }
        clave_valor->valor = dato;
        return true;
    }
    if ((float)hash->cantidad/(float)hash->tamanio > FACTOR_CARGA){
        hash = _reasignar_posiciones_hash(hash, siguiente_primo(hash->tamanio*2));
    }
    clave_valor = _crear_clave_valor(clave, dato);
    return _hash_guardar(hash, clave_valor);
}

void *hash_borrar(hash_t *hash, const char *clave){
    long posicion;
    void* valor = NULL;
    for (int i = 0; i < CANTIDAD_FUNCIONES_HASH && valor == NULL; i++) {
        posicion = funciones_hash[i](clave, hash->tamanio);
        lista_t* bucket = hash->tabla[posicion];
        clave_valor_t* actual;
        lista_iter_t* iter = lista_iter_crear(bucket);
        while (!lista_iter_al_final(iter)){
            actual = lista_iter_ver_actual(iter);
            if(strcmp(clave, actual->clave) == 0){
                valor = actual->valor;
                free(actual->clave);
                free(lista_iter_borrar(iter));
                hash->cantidad--;
                break;
            }
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
    }
    if (hash->cantidad > 100 && hash->tamanio/hash->cantidad > 3){
        _reasignar_posiciones_hash(hash, siguiente_primo(hash->tamanio/2));
    }
    return valor;
}

hash_iter_t *hash_iter_crear(const hash_t *hash) {
    hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
    if (!iterador_hash) {
        return NULL;
    }
    iterador_hash->hash = hash;
    iterador_hash->posicion_tabla = 0;
    iterador_hash->iter_bucket_actual = lista_iter_crear(hash->tabla[0]);;
    iterador_hash->recorridos = 0;
    if (lista_iter_al_final(iterador_hash->iter_bucket_actual)){
        hash_iter_avanzar(iterador_hash);
    }
    return iterador_hash;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
    if(iter->hash->cantidad == 0 || iter->recorridos >= iter->hash->cantidad
       || iter->posicion_tabla >= iter->hash->tamanio){
        iter->recorridos++;
        lista_iter_avanzar(iter->iter_bucket_actual);
        return false;
    }
    lista_iter_avanzar(iter->iter_bucket_actual);
    while (lista_iter_al_final(iter->iter_bucket_actual)){
        iter->posicion_tabla++;
        if (iter->posicion_tabla < iter->hash->tamanio){
            lista_iter_destruir(iter->iter_bucket_actual);
            iter->iter_bucket_actual = lista_iter_crear(iter->hash->tabla[iter->posicion_tabla]);
        }
        else{
            lista_iter_avanzar(iter->iter_bucket_actual);
            iter->recorridos++;
            return false;
        }
    }
    iter->recorridos++;
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
    clave_valor_t* clave_valor = lista_iter_ver_actual(iter->iter_bucket_actual);
    if(clave_valor == NULL){
        return NULL;
    }
    return clave_valor->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
    return iter->recorridos > iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter) {
    lista_iter_destruir(iter->iter_bucket_actual);
    free(iter);
}

hash_t* _hash_crear(hash_destruir_dato_t destruir_dato, size_t capacidad){
    hash_t* hash = malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->tamanio = capacidad;
    hash->cantidad = 0;
    hash->tabla = malloc(sizeof(lista_t*)*capacidad);
    if (hash->tabla == NULL){
        free(hash);
        return NULL;
    }
    for (int i = 0; i < hash->tamanio; i++) {
        hash->tabla[i] = lista_crear();
    }
    return hash;
}

bool _hash_guardar(hash_t *hash, clave_valor_t* clave_valor) {
    funcion_hash_t funcion_hash = funciones_hash[clave_valor->cantidad_hash_aplicado];
    clave_valor->cantidad_hash_aplicado++;
    char* clave = clave_valor->clave;
    long posicion = funcion_hash(clave, hash->tamanio);
    lista_t* bucket = hash->tabla[posicion];
    lista_iter_t* iter;
    if (lista_largo(bucket) < CAPACIDAD_BUCKETS){
        iter = lista_iter_crear(bucket);
        lista_iter_insertar(iter, clave_valor);
        lista_iter_destruir(iter);
        hash->cantidad++;
        return true;
    }
    else{
        iter = lista_iter_crear(bucket);
        clave_valor_t* primer_elemento = lista_iter_borrar(iter);
        lista_iter_insertar(iter, clave_valor);
        lista_iter_destruir(iter);
        return _hash_guardar(hash,primer_elemento);
    }
}

void _recorrer_buckets(hash_t* hash, bool visitar(lista_iter_t* bucket, void* dato, hash_destruir_dato_t destruir_dato)
        , void* dato, hash_destruir_dato_t destruir_dato){
    for (int i = 0; i < hash->tamanio; i++) {
        lista_iter_t* iter_bucket = lista_iter_crear(hash->tabla[i]);
        visitar(iter_bucket, dato, destruir_dato);
        lista_iter_destruir(iter_bucket);
        lista_destruir(hash->tabla[i], NULL);
    }
}

hash_t* _reasignar_posiciones_hash(hash_t* hash, size_t capacidad){
    hash_t* nuevo_hash = _hash_crear(hash->destruir_dato, capacidad);
    _recorrer_buckets(hash,_guardar_nuevo_hash,nuevo_hash,NULL);
    free(hash->tabla);
    *hash = *nuevo_hash;
    free(nuevo_hash);
    return hash;
}

bool _guardar_nuevo_hash(lista_iter_t* iter_bucket, void* nuevo_hash, hash_destruir_dato_t destruir_dato){
    hash_t* hash = nuevo_hash;
    while (!lista_iter_al_final(iter_bucket)){
        clave_valor_t* clave_valor = lista_iter_ver_actual(iter_bucket);
        hash_guardar(hash, clave_valor->clave, clave_valor->valor);
        free(clave_valor->clave);
        free(clave_valor);
        lista_iter_avanzar(iter_bucket);
    }
    return true;
}

bool _destruir_bucket(lista_iter_t* iter_bucket, void* dato, hash_destruir_dato_t destruir_dato){
    while (!lista_iter_al_final(iter_bucket)){
        clave_valor_t* clave_valor = lista_iter_borrar(iter_bucket);
        if(destruir_dato != NULL){
            destruir_dato(clave_valor->valor);
        }
        free(clave_valor->clave);
        free(clave_valor);
    }
    return true;
}


clave_valor_t* _buscar(const char* clave, const hash_t* hash){
    long posicion;
    clave_valor_t* actual;
    for (int i = 0; i < CANTIDAD_FUNCIONES_HASH; i++) {
        posicion = funciones_hash[i](clave, hash->tamanio);
        lista_t *bucket = hash->tabla[posicion];
        lista_iter_t *iter = lista_iter_crear(bucket);
        while (!lista_iter_al_final(iter)) {
            actual = lista_iter_ver_actual(iter);
            if (strcmp(clave, actual->clave) == 0) {
                lista_iter_destruir(iter);
                return actual;
            }
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
    }
    return NULL;
}

clave_valor_t* _crear_clave_valor(const char* clave, void* dato){
    clave_valor_t* clave_valor = malloc(sizeof(clave_valor_t));
    if(clave_valor == NULL){
        return NULL;
    }
    clave_valor->cantidad_hash_aplicado = 0;
    clave_valor->clave = malloc(sizeof(char)*(strlen(clave)+1));
    if(clave_valor->clave == NULL){
        free(clave_valor);
        return NULL;
    }
    strcpy(clave_valor->clave, clave );
    clave_valor->valor = dato;
    return clave_valor;
}

