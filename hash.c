#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"
#include "string.h"

#define TAMANIO 97;
//#define BUCKETS 3;
const int BUCKETS = 3;
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
struct hash_iter{
    hash_t* hash;
    size_t indice_lista_actual;
    clave_valor_t* actual;
    lista_iter_t* iter_lista_actual;
};

typedef struct hash hash_t;

typedef int*(*puntero_hash_t)(void*);

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->tamanio = TAMANIO;
    hash->cantidad = 0;
    hash->tabla = malloc(sizeof(lista_t*));
    if (hash->tabla == NULL){
        free(hash);
        return NULL;
    }
    return hash;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    for (int i = 0; i < hash_cantidad(hash); i++) {
        lista_destruir(hash->tabla[i],hash->destruir_dato);
    }
    free(hash->tabla);
    free(hash);
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave) {
    int posicion = func_hash();
    clave_valor_t* actual;
    hash->tabla[posicion];
    lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);
    while (!lista_iter_al_final(iter)){
        actual = lista_iter_ver_actual(iter);
        if (strcmp(clave, actual->clave) == 0){
            return true;
        }
    }
	return false;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

	int valor = hash_01(clave);
	if (hash->tabla[valor] == NULL) {
        hash->tabla[valor] = lista_crear();
	}
	clave_valor_t* actual;
	clave_valor_t* clave_valor = malloc(sizeof(clave_valor_t));
	strcpy(clave_valor->clave, clave );
	clave_valor->valor = dato;
	lista_iter_t* iter = lista_iter_crear(hash->tabla[valor]);
    for (int i=0; i < BUCKETS; i++){
	    actual = lista_iter_ver_actual(iter);
	    if(strcmp(actual->clave, clave) == 0){
	        lista_iter_borrar(iter);
	        lista_iter_insertar(iter, clave_valor);
            return true;
	    }
	    lista_iter_avanzar(iter);
	}
	if(lista_iter_al_final(iter)){

    } else{
        lista_iter_insertar(iter, clave_valor);
        lista_iter_destruir(iter);
        hash->cantidad++;
        return true;
	}
}

void *hash_borrar(hash_t *hash, const char *clave){
    int valor = hash_01(clave);
    if (hash->tabla[valor] == NULL) {
        return NULL;
    }
    clave_valor_t* actual;
    lista_iter_t* iter = lista_iter_crear(hash->tabla[valor]);
    while (!lista_iter_al_final(iter)){

    }
}

int hash_01(void* clave) {
	//Poner código FNV Hashing
	return 4
}

int* hash_02(void* clave) {
	//Jenkins Hashing
}



// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash_) {
	hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
	if (!iterador_hash) {
		return NULL;
	}
	iterador_hash->hash = hash_;
	iterador_hash->actual = lista_ver_primero(hash->tabla[0]);
	return true;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter) {

for (lista_principal, i++){
	lista_clave_valor = lista_principal[i]
	crear_iterador(list_clave_valor)
	while(iterador_clave_valor)
		itera_avanzar
	destruir(iter)
}


}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	//consulta porque en iter->actual tengo una listal, y muchos elementos.
	lista_iter_t *iterador = lista_iter_crear( iter->actual );
	return(lista_ver_primero(iter->actual)->clave);
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter) {
	while ( hash_iter_avanzar(iter) );
	return true;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}
