#include "lista.h"
#include <stdlib.h>

typedef struct nodo{
    void** dato;
    struct nodo* siguiente;
} nodo_t;
struct lista{
    nodo_t* primero;
    nodo_t* ultimo;
    size_t largo;
};
struct lista_iter{
    nodo_t* anterior;
    nodo_t* actual;
    lista_t* lista;
};

nodo_t* _crear_inicializar_nodo(void *dato);
void _iterar_nodos(nodo_t* nodo, bool visitar(void *dato, void *extra), void *extra);

lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if(lista == NULL){
        return NULL;
    }
    lista->primero = NULL;
    lista->ultimo = NULL;
    lista->largo = 0;
    return lista;
}

bool lista_esta_vacia(const lista_t *lista){
    return lista->largo == 0;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo = _crear_inicializar_nodo(dato);
    if(nodo == NULL){
        return false;
    }
    if(lista_esta_vacia(lista)){
        lista->ultimo = nodo;
    }
    else{
        nodo->siguiente = lista->primero;
    }
    lista->primero = nodo;
    lista->largo++;
    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo = _crear_inicializar_nodo(dato);
    if(nodo == NULL){
        return false;
    }
    if(lista_esta_vacia(lista)){
        lista->primero = nodo;
    }
    else{
        lista->ultimo->siguiente = nodo;
    }
    lista->ultimo = nodo;
    lista->largo++;
    return true;
}

void* lista_borrar_primero(lista_t *lista){
    if(lista_esta_vacia(lista)){
        return NULL;
    }
    void* dato = lista_ver_primero(lista);
    nodo_t* primero = lista->primero;
    lista->primero = lista->primero->siguiente;
    free(primero);
    lista->largo--;
    return dato;
}

void* lista_ver_primero(const lista_t *lista){
    if(lista_esta_vacia(lista)){
        return NULL;
    }
    return lista->primero->dato;
}

void* lista_ver_ultimo(const lista_t* lista){
    if(lista_esta_vacia(lista)){
        return NULL;
    }
    return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
    return lista->largo;
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)){
    void* dato;
    while (!lista_esta_vacia(lista)){
        dato = lista_borrar_primero(lista);
        if(destruir_dato != NULL){
            destruir_dato(dato);
        }
    }
    free(lista);
}

//ITERADOR INTERNO

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
    if(!lista_esta_vacia(lista))
        _iterar_nodos(lista->primero, visitar, extra);
}

void _iterar_nodos(nodo_t* nodo, bool visitar(void *dato, void *extra), void *extra){
    if(visitar(nodo->dato,extra) && nodo->siguiente != NULL)
        _iterar_nodos(nodo->siguiente, visitar, extra);
}

//ITERADOR EXTERNO

lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t* lista_iter = malloc(sizeof(lista_iter_t));
    if(lista_iter == NULL)
        return  NULL;

    lista_iter->actual = lista->primero;
    lista_iter->anterior = NULL;
    lista_iter->lista = lista;
    return lista_iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
    if(lista_iter_al_final(iter)){
        return false;
    }
    iter->anterior = iter->actual;
    iter->actual = iter->actual->siguiente;
    return true;
}

void* lista_iter_ver_actual(const lista_iter_t *iter){
    if(lista_iter_al_final(iter))
        return NULL;
    return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t* iter){
    return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter){
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nuevo_nodo = _crear_inicializar_nodo(dato);
    if(nuevo_nodo == NULL){
        return false;
    }
    if(iter->anterior == NULL){
        nuevo_nodo->siguiente = iter->lista->primero;
        iter->lista->primero = nuevo_nodo;
        if(lista_esta_vacia(iter->lista))
            iter->lista->ultimo = nuevo_nodo;
        iter->actual = nuevo_nodo;
        iter->anterior = NULL;
    }
    else if(iter->actual == NULL){
        iter->anterior = iter->lista->ultimo;
        iter->actual = nuevo_nodo;
        iter->lista->ultimo->siguiente = nuevo_nodo;
        iter->lista->ultimo = nuevo_nodo;
    }
    else{
        nuevo_nodo->siguiente = iter->actual;
        iter->anterior->siguiente = nuevo_nodo;
        iter->actual = nuevo_nodo;
    }
    iter->lista->largo++;
    return true;
}

void* lista_iter_borrar(lista_iter_t *iter){
    if(lista_esta_vacia(iter->lista) || iter->actual == NULL)
        return NULL;
    nodo_t* actual = iter->actual;
    void* dato = actual->dato;
    if(iter->anterior == NULL){
        iter->lista->primero = iter->actual->siguiente;
        iter->actual = iter->lista->primero;
        iter->anterior = NULL;
    }
    else if(iter->actual->siguiente == NULL){
        if(iter->actual == NULL){
            return NULL;
        }
        iter->actual = NULL;
        iter->anterior->siguiente = NULL;
        iter->lista->ultimo = iter->anterior;
    }
    else{
        iter->actual = iter->actual->siguiente;
        iter->anterior->siguiente = iter->actual;
    }
    iter->lista->largo--;
    free(actual);
    return dato;
}

//FUNCIONES AUXILIARES

nodo_t* _crear_inicializar_nodo(void *dato){
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if(nodo == NULL){
        return NULL;
    }
    nodo->dato = dato;
    nodo->siguiente = NULL;
    return nodo;
}