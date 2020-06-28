#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"
#include <string.h>
#include <stdint.h>
#include "math.h"
#define FNV_PRIME_64 1099511628211U
#define FNV_OFFSET_64 14695981039346656037U

const size_t TAMANIO = 997;//5003 997
#define CAPACIDAD_BUCKETS 4

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
    size_t indice_lista_actual;
    size_t recorridos;
    clave_valor_t* clave_valor;
    lista_iter_t* iter_posicion_actual;
};

typedef size_t(*funcion_hash_t)(const char*, size_t);
size_t hash_1(const char *, size_t);
size_t hash_02(const char*, size_t);
size_t hash_03(const char*, size_t);
funcion_hash_t funciones_hash[3] = {hash_1, hash_02, hash_03};
bool isPrime(size_t prime);
size_t siguiente_primo(size_t numero);

hash_t* _reasignar_posiciones_hash(hash_t* hash);
hash_t* _hash_crear(hash_destruir_dato_t destruir_dato, size_t capacidad);
bool _hash_guardar(hash_t *hash, clave_valor_t* claveValor);

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
    return _hash_crear(destruir_dato, TAMANIO);
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

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    lista_t* bucket;
    for (int i = 0; i < hash->tamanio; i++) {
        bucket = hash->tabla[i];
        lista_iter_t* iter = lista_iter_crear(bucket);
        while (!lista_iter_al_final(iter)){
            clave_valor_t* clave_valor = lista_iter_borrar(iter);
            if(hash->destruir_dato != NULL){
                hash->destruir_dato(clave_valor->valor);
            }
            free(clave_valor->clave);
            free(clave_valor);

        }
        lista_iter_destruir(iter);
        lista_destruir(bucket, NULL);
    }
    free(hash->tabla);
    free(hash);
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
    long posicion;
    clave_valor_t* actual;
    bool pertenece = false;
    for (int i = 0; i < 3 && !pertenece; i++) {
        posicion = funciones_hash[i](clave, hash->tamanio);
        lista_iter_t *iter = lista_iter_crear(hash->tabla[posicion]);
        while (!lista_iter_al_final(iter)) {
            actual = lista_iter_ver_actual(iter);
            if (strcmp(clave, actual->clave) == 0) {
                pertenece = true;
                break;
            }
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
    }
    return pertenece;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    long posicion;
    clave_valor_t* actual;
    void* valor = NULL;
    for (int i = 0; i < 3 && valor == NULL; i++) {
        posicion = funciones_hash[i](clave, hash->tamanio);
        lista_t *bucket = hash->tabla[posicion];
        lista_iter_t *iter = lista_iter_crear(bucket);
        while (!lista_iter_al_final(iter)) {
            actual = lista_iter_ver_actual(iter);
            if (strcmp(clave, actual->clave) == 0) {
                valor = actual->valor;
                break;
            }
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
    }
    return valor;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if(hash->cantidad == hash->tamanio){
        hash = _reasignar_posiciones_hash(hash);
    }
    clave_valor_t* clave_valor = malloc(sizeof(clave_valor_t));
    if(clave_valor == NULL){
        return false;
    }
    clave_valor->cantidad_hash_aplicado = 0;
    clave_valor->clave = malloc(sizeof(char)*(strlen(clave)+1));
    if(clave_valor->clave == NULL){
        free(clave_valor);
        return false;
    }
    strcpy(clave_valor->clave, clave );
    clave_valor->valor = dato;
    return _hash_guardar(hash, clave_valor);
}

bool _hash_guardar(hash_t *hash, clave_valor_t* clave_valor) {
    if(clave_valor->cantidad_hash_aplicado >= 3){
        hash = _reasignar_posiciones_hash(hash);
        clave_valor->cantidad_hash_aplicado = 0;
    }
    funcion_hash_t funcion_hash = funciones_hash[clave_valor->cantidad_hash_aplicado];
    clave_valor->cantidad_hash_aplicado++;
    char* clave = clave_valor->clave;
    long posicion = funcion_hash(clave, hash->tamanio);
    lista_t* bucket = hash->tabla[posicion];
    clave_valor_t* actual;
    lista_iter_t* iter;
    if (lista_largo(bucket) < CAPACIDAD_BUCKETS){
        iter = lista_iter_crear(bucket);
        for (int i = 0; i < CAPACIDAD_BUCKETS && !lista_iter_al_final(iter); i++) {
            actual = lista_iter_ver_actual(iter);
            if (strcmp(actual->clave, clave) == 0){
                if(hash->destruir_dato != NULL){
                    hash->destruir_dato(actual->valor);
                }
                free(actual->clave);
                free(lista_iter_borrar(iter));
                hash->cantidad--;
                break;
            }
            lista_iter_avanzar(iter);
        }
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

void *hash_borrar(hash_t *hash, const char *clave){
    long posicion;
    void* valor = NULL;
    for (int i = 0; i < 3 && valor == NULL; i++) {
        posicion = funciones_hash[i](clave, hash->tamanio);
        lista_t* bucket = hash->tabla[posicion];
        clave_valor_t* actual;
        lista_iter_t* iter = lista_iter_crear(bucket);
        while (!lista_iter_al_final(iter)){
            actual = lista_iter_ver_actual(iter);
            if(strcmp(clave, actual->clave) == 0){
                valor = actual->valor;
                if(hash->destruir_dato != NULL){
                    hash->destruir_dato(actual->valor);
                }
                free(actual->clave);
                free(lista_iter_borrar(iter));
                hash->cantidad--;
                break;
            }
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
    }
    return valor;
}

hash_t* _reasignar_posiciones_hash(hash_t* hash){
    hash_t* nuevo_hash = _hash_crear(hash->destruir_dato, siguiente_primo(hash->tamanio+TAMANIO));
    lista_t* clave_valor_lista = lista_crear();
    for (int i = 0; i < hash->tamanio; i++) {
        lista_iter_t* iter_bucket = lista_iter_crear(hash->tabla[i]);
        while (!lista_iter_al_final(iter_bucket)){
            lista_insertar_primero(clave_valor_lista, lista_iter_borrar(iter_bucket));
        }
        lista_iter_destruir(iter_bucket);
        lista_destruir(hash->tabla[i], NULL);
    }
    free(hash->tabla);
    while (!lista_esta_vacia(clave_valor_lista)){
        clave_valor_t* clave_valor = lista_borrar_primero(clave_valor_lista);
        hash_guardar(nuevo_hash, clave_valor->clave, clave_valor->valor);
        free(clave_valor->clave);
        free(clave_valor);
    }
    *hash = *nuevo_hash;
    lista_destruir(clave_valor_lista, NULL);
    free(nuevo_hash);
    return hash;
}

size_t hash_1(const char *str, size_t tamanio) {	//Borrar este comentario: pongo size_t porque el largo de la tabla hash no va a superar el límite, y estamos haciendo módulo a la longitud de la tabla...
    unsigned long hash = 5381;
    int c = *str;
    while (c) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        c = *str++;
    }
    return hash % tamanio;
}

int hash_01(void* clave, size_t largo_tabla_hash) {
    //código FNV Hashing. Ref: http://ctips.pbworks.com/w/page/7277591/FNV%20Hash
    size_t n = strlen((char*) clave);
    uint_fast64_t hash = FNV_OFFSET_64;
    for(size_t i = 0; i < n; i++) {
        hash = hash ^ (((unsigned char*)clave)[i]); 		// xor next byte into the bottom of the hash
        hash = hash * FNV_PRIME_64; 	// Multiply by prime number found to work well
    }
    return (int)(hash%largo_tabla_hash);
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
/*
int hash_murmur(void* clave, size_t largo_tabla_hash) {
	// Murmur Hashing
	// para tener en cuenta: https://stackoverflow.com/questions/7666509/hash-function-for-string?rq=1
	uint_fast64_t h = 525201411107845655ull;
	  for (;*clave;++clave) {
	    h ^= *clave;
	    h *= 0x5bd1e9955bd1e995;
	    h ^= h >> 47;
	  }
	  return h%largo_tabla_hash;
}
*/
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

hash_iter_t *hash_iter_crear(const hash_t *hash) {
    hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
    if (!iterador_hash) {
        return NULL;
    }
    iterador_hash->hash = hash;
    iterador_hash->indice_lista_actual = 0;
    lista_iter_t* iter_bucket = lista_iter_crear(hash->tabla[0]);
    while (lista_iter_al_final(iter_bucket) && iterador_hash->indice_lista_actual < iterador_hash->hash->tamanio-1){
        lista_iter_destruir(iter_bucket);
        iterador_hash->indice_lista_actual++;
        iter_bucket = lista_iter_crear(hash->tabla[iterador_hash->indice_lista_actual]);
    }
    iterador_hash->iter_posicion_actual = iter_bucket;
    iterador_hash->clave_valor = lista_iter_ver_actual(iterador_hash->iter_posicion_actual);
    iterador_hash->recorridos = 0;
    return iterador_hash;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
    if(iter->hash->cantidad == 0 || iter->recorridos >= iter->hash->cantidad){
        return false;
    }
    clave_valor_t* actual =  lista_iter_ver_actual(iter->iter_posicion_actual);
    if(!lista_iter_al_final(iter->iter_posicion_actual)){
        if (strcmp(actual->clave, iter->clave_valor->clave) == 0){
            lista_iter_avanzar(iter->iter_posicion_actual);
        }
        actual =  lista_iter_ver_actual(iter->iter_posicion_actual);
        if (actual == NULL && iter->recorridos < iter->hash->cantidad-1){
            return hash_iter_avanzar(iter);
        }
        iter->clave_valor = actual;
        iter->recorridos++;
        return true;
    }
    else {
        lista_iter_destruir(iter->iter_posicion_actual);
        iter->indice_lista_actual++;
        iter->iter_posicion_actual = lista_iter_crear(iter->hash->tabla[iter->indice_lista_actual]);
        return hash_iter_avanzar(iter);
    }
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
    clave_valor_t* clave_valor = iter->clave_valor;
    if(clave_valor == NULL){
        return NULL;
    }
    return clave_valor->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
    return iter->recorridos >= iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter) {
    lista_iter_destruir(iter->iter_posicion_actual);
    free(iter);
}

size_t siguiente_primo(size_t numero){
    if(numero%2 == 0 && numero != 2){
        numero+=1;
    }

    /* while its not a prime number, check the next odd number */
    while(!isPrime(numero)){
        numero+=2;
    }
    return numero;
}

bool isPrime(size_t prime){
    int i;
    if(prime == 2){
        return true;
    }

    if(prime%2 == 0 || prime <= 1){
        return false;
    } else {
        for(i=3; i<=sqrt((double)prime); i+=2){
            if(prime%i == 0){
                return false;
            }
        }
    }
    return true;
}