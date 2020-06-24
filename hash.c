#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"
#include "string.h"

const int TAMANIO = 97;
const int BUCKETS = 3;
typedef struct {
    char* clave;
    void* valor;
}clave_valor_t;
struct hash{
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
};
struct hash_iter{
    hash_t* hash;
    size_t indice_lista_actual;
    clave_valor_t* actual;
    lista_iter_t* iter_lista_actual;
};

typedef int*(*puntero_hash_t)(void*);
unsigned long hash_1(const char *str);

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->tamanio = TAMANIO;
    hash->cantidad = 0;
    hash->tabla = malloc(sizeof(lista_t*)*TAMANIO);
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
        lista_destruir(bucket, hash->destruir_dato);
    }
    free(hash->tabla);
    free(hash);
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
    long posicion = hash_1(clave);
    clave_valor_t* actual;
    lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);
    while (!lista_iter_al_final(iter)){
        actual = lista_iter_ver_actual(iter);
        if (strcmp(clave, actual->clave) == 0){
            lista_iter_destruir(iter);
            return true;
        }
    }
    lista_iter_destruir(iter);
	return false;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    long posicion = hash_1(clave);
    clave_valor_t* actual;
    lista_t* bucket = hash->tabla[posicion];
    lista_iter_t* iter = lista_iter_crear(bucket);
    while (!lista_iter_al_final(iter)){
        actual = lista_iter_ver_actual(iter);
        if (strcmp(clave, actual->clave) == 0){
            lista_iter_destruir(iter);
            return actual->valor;
        }
    }
    lista_iter_destruir(iter);
    return NULL;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

	long valor = hash_1(clave);
    lista_t* bucket = hash->tabla[valor];
	clave_valor_t* actual;
	clave_valor_t* clave_valor = malloc(sizeof(clave_valor_t));
	if(clave_valor == NULL){
	    return false;
	}
	clave_valor->clave = malloc(sizeof(char)*strlen(clave));
	if(clave_valor->clave = NULL){
	    free(clave_valor);
	    return false;
	}
	strcpy(clave_valor->clave, clave );
	clave_valor->valor = dato;
	lista_iter_t* iter;
	if (lista_largo(bucket) < BUCKETS){
        iter = lista_iter_crear(bucket);
        for (int i = 0; i < BUCKETS && !lista_iter_al_final(iter); i++) {
            actual = lista_iter_ver_actual(iter);
            if (strcmp(actual->clave, clave) == 0){
                free(actual->clave);
                lista_iter_borrar(iter);
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
        return hash_guardar(hash,primer_elemento->clave, primer_elemento->valor);
	}
}

void *hash_borrar(hash_t *hash, const char *clave){
    long posicion = hash_1(clave);
    lista_t* bucket = hash->tabla[posicion];
    if (bucket == NULL) {
        return NULL;
    }
    clave_valor_t* actual;
    lista_iter_t* iter = lista_iter_crear(bucket);
    while (!lista_iter_al_final(iter)){
        actual = lista_iter_ver_actual(iter);
        if(strcmp(clave, actual->clave) == 0){
            free(actual->clave);
            lista_iter_borrar(iter);
            return actual->valor;
        }
        lista_iter_avanzar(iter);
    }
    return NULL;
}

unsigned long hash_1(const char *str)
{
    unsigned long hash = 5381;
    int c = *str;

    while (c){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        c = *str++;
    }


    return hash%TAMANIO;
}

int* hash_02(void* clave) {
	//Jenkins Hashing
	return 0;
}



// Crea iterador
/*hash_iter_t *hash_iter_crear(const hash_t *hash_) {
	hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
	if (!iterador_hash) {
		return NULL;
	}
	iterador_hash->hash = hash_;
	iterador_hash->actual = lista_ver_primero(hash->tabla[0]);
	return true;
}*/

// Avanza iterador
/*bool hash_iter_avanzar(hash_iter_t *iter) {

for (lista_principal, i++){
	lista_clave_valor = lista_principal[i]
	crear_iterador(list_clave_valor)
	while(iterador_clave_valor)
		itera_avanzar
	destruir(iter)
}


}*/

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
/*const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	//consulta porque en iter->actual tengo una listal, y muchos elementos.
	lista_iter_t *iterador = lista_iter_crear( iter->actual );
	return(lista_ver_primero(iter->actual)->clave);
}*/

// Comprueba si terminó la iteración
/*bool hash_iter_al_final(const hash_iter_t *iter) {
	while ( hash_iter_avanzar(iter) );
	return true;
}*/

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}
