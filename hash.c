#include "hash.h"
#include <stddef.h>
#include "stdlib.h"
#include "lista.h"
#include <string.h>
#include <stdint.h>
#define FNV_PRIME_64 1099511628211
#define FNV_OFFSET_64 14695981039346656037

#define TAMANIO 97;
#define CAPACIDAD_BUCKETS_TABLA_HASH 4
#define CAPACIDAD_INICIAL_TABLA_HASH 50
typedef struct {
    void* clave;
    void* valor;
    size_t numero_hash;
    bool reubicando;
}clave_valor_t;
struct hash {
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
};
struct hash_iter {
    hash_t* hash;
    size_t* indice_lista_actual;
   // clave_valor_t* actual;
    lista_iter_t* iter_posicion_actual;
};

typedef int*(*puntero_funcion_hash_t)(void*);

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->tamanio = TAMANIO;
    hash->cantidad = 0;
    hash->tabla = malloc(sizeof(lista_t*));
    *hash->tabla = lista_crear();
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
	hash_iter_t *iter = hash_iter_crear(hash);
	if (!iter) {
		return false;
	}
	while(strcmp(hash_iter_ver_actual(iter),clave)) {
		if (! hash_iter_avanzar(iter) ) {
			return false;
		}
	}
	return true;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	/*int * valor = hash_01(clave);
	lista_t* listita = hash->tabla[valor];
	if (!listita) {
		listita = ista_crear();
	}

	clave_valor_t* clave_valor = malloc(sizeof(clave_valor_t));
	strcpy(clave_valor->clave, clave );
	clave_valor->valor = dato;
	lista_insertar_ultimo( hash->tabla[valor], clave_valor);*/

	puntero_funcion_hash_t* array_funciones_hash = {hash_01, hash_02, hash_03, NULL};
	int flag;
	size_t resultado_hash, i = 0;
	lista_iter_t *iterador = NULL;
	clave_valor_t *par_clave_valor = malloc(sizeof(clave_valor_t));
	if (!par_clave_valor) {
		return false;
	}
	par_clave_valor->clave = clave;
	par_clave_valor->valor = dato;
	par_clave_valor->numero_hash = 01;
	par_clave_valor->reubicando = false;
	resultado_hash = array_funciones_hash[i](clave, hash->cantidad);
	if (lista_largo(hash->tabla[resultado_hash]) == CAPACIDAD_BUCKETS_TABLA_HASH) {
		clave_valor_t *par_desplazado = lista_borrar_primero(hash->tabla[resultado_hash]);
		par_desplazado->reubicando = true;
		lista_insertar_ultimo(hash->tabla[resultado_hash], par_clave_valor);
		return reubicar(hash, par_desplazado);
	} else {
		return lista_insertar_ultimo(hash->tabla[resultado_hash], par_clave_valor);
	}
}

int hash_01(void* clave, size_t largo_tabla_hash) {
	//código FNV Hashing. Ref: http://ctips.pbworks.com/w/page/7277591/FNV%20Hash
	size_t n = strlen((char*) clave);
		uint_fast64_t hash = FNV_OFFSET_64;
	    for(size_t i = 0; i < n; i++) {
	        hash = hash ^ (clave[i]); 		// xor next byte into the bottom of the hash
	        hash = hash * FNV_PRIME_64; 	// Multiply by prime number found to work well
	    }
	    return hash % largo_tabla_hash;
}
/*	char *teststring = "This is a test";
	uint32_t hash_of_string = FNV32(teststring);
*/

int hash_02(void* clave, size_t largo_tabla_hash) {
	//Jenkins Hashing
	//Ref: https://en.wikipedia.org/wiki/Jenkins_hash_function
	  size_t i = 0, n = strlen((char*)clave);
	  uint32_t hash = 0;
	  while (i != n) {
	    hash += clave[i++];
	    hash += hash << 10;
	    hash ^= hash >> 6;
	  }
	  hash += hash << 3;
	  hash ^= hash >> 11;
	  hash += hash << 15;
	  return hash%largo_tabla_hash;
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
int hash_03(const void * clave, int largo_tabla_hash) {
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
		h ^= uint64(data2[6]) << 48;
		break;
	case 6:
		h ^= uint64(data2[5]) << 40;
		break;
	case 5:
		h ^= uint64(data2[4]) << 32;
		break;
	case 4:
		h ^= uint64(data2[3]) << 24;
		break;
	case 3:
		h ^= uint64(data2[2]) << 16;
		break;
	case 2:
		h ^= uint64(data2[1]) << 8;
		break;
	case 1:
		h ^= uint64(data2[0]);
		h *= m;
		break;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h%largo_tabla_hash;
}

// Crea iterador
// pre: el tda hash ha sido creado.
// post: el tda iterador de hash ha sido creado e inicializado.
hash_iter_t *hash_iter_crear(const hash_t *hash) {
	hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
	if (!iterador_hash) {
		return NULL;
	}
	iterador_hash->hash = hash;
	iterador_hash->iter_posicion_actual = lista_iter_crear(hash->tabla[0]);
	iterador_hash->indice_lista_actual = 0;
	// iterador_hash->actual = lista_iter_ver_actual(iterador_hash->iter_posicion_actual);
	return true;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter) {
// fijarnos de poner una
// #define CAPACIDAD_BUCKETS_TABLA_HASH 4 y una #define CAPACIDAD_INICIAL_TABLA_HASH 50

	if ( lista_iter_avanzar(iter->iter_posicion_actual) && !lista_iter_al_final(iter->iter_posicion_actual) ) {
		return true;
	} else if (iter->indice_lista_actual < iter->hash->tamanio - 1){
		lista_iter_destruir(iter->iter_posicion_actual);
		iter->indice_lista_actual++;
		iter->iter_posicion_actual = lista_iter_crear(iter->hash->tabla[iter->indice_lista_actual]);
		return true;
	} else {
		return false;
	}
/*
for (lista_principal, i++){
	lista_clave_valor = lista_principal[i]
	crear_iterador(list_clave_valor)
	while(iterador_clave_valor)
		itera_avanzar
	destruir(iter)
}
*/
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	return( lista_iter_ver_actual(iter->iter_posicion_actual) );
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter) {
	return lista_iter_al_final(iter->iter_posicion_actual) && iter->indice_lista_actual == iter->hash->tamanio - 1;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}
