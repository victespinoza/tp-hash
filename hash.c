typedef struct {
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
}

typedef void (*hash_destruir_dato_t)(void*);