#ifndef PILA_LISTA_H
#define PILA_LISTA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct lista lista_t;
/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al comienzo de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al comienzo
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el tamaño de la lista
// Pre: la lista fue creada
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
typedef struct lista_iter lista_iter_t;

// Crea una iterador externo.
// Post: devuelve un nuevo iterador con el primer elemento de la lista como elemento actual.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Actualiza el elemento actual al siguiente elemento, si el elemento actual existe
// Pre: el iterador fue creado
// Post: devuelve true si se pudo cambiar el elemento actual por el siguiente,
// caso contrario devuelve false.
bool lista_iter_avanzar(lista_iter_t *iter);

//Retorna el valor del elemento actual
// Pre: el iterador fue creado
// Post: se devuelvio el valor del elemento actual
void* lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve el valor de verdad al evaluar si el iterador se encuentra fuera del rango
//de la lista
// Pre: el iterador fue creado
// Post: Devolvio true si el elemento actual se encuentra dentro del rango de la lista,
// caso contrario devuelve false.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador externo.
// Pre: el iterador fue creado.
// Post: se eliminaro el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Agrega un nuevo elemento en la posicion actual del iterador. Devuelve falso en caso de error.
// Pre: el iterador fue creado.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra en la posicion
// del elemento actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Saca el elemento actual del iterador en la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_iter_borrar(lista_iter_t *iter);

#endif //PILA_LISTA_H
