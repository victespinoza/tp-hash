#define  _POSIX_C_SOURCE 200809L
#include "testing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
typedef struct lista lista_t;
struct hash {
    lista_t** tabla;
    size_t tamanio;
    size_t cantidad;
    hash_destruir_dato_t destruir_dato;
};
typedef struct nodo{
    void** dato;
    struct nodo* siguiente;
} nodo_t;
struct lista{
    nodo_t* primero;
    nodo_t* ultimo;
    size_t largo;
};
void pruebas_hash_catedra(void);
void pruebas_volumen_catedra(size_t);

/*int main(int argc, char *argv[])
{
    if (argc > 1) {
        // Asumimos que nos están pidiendo pruebas de volumen.
        long largo = strtol(argv[1], NULL, 10);
        pruebas_volumen_catedra((size_t) largo);

        return failure_count() > 0;
    }

    printf("\n~~~ PRUEBAS CÁTEDRA ~~~\n");
    pruebas_hash_catedra();



    return failure_count() > 0;
}*/
extern void destruir_clave_valor(void *tda);

int main(int argc, char* argv[]){
    /*hash_t* hash = hash_crear(destruir_clave_valor);
    printf("Guardando hash pepito\n");
    hash_guardar(hash, "pepito","valor");
    printf("Guardando hash pepito2\n");
    hash_guardar(hash, "pepito2","valor2");
    printf("Guardando hash pepito3\n");
    hash_guardar(hash, "pepito3","valor3");
    printf("Guardando hash pepito con valor actualizado\n");
    hash_guardar(hash, "pepito","valor actualizado");
    printf("Borrando hash pepito2\n");
    hash_borrar(hash, "pepito2");
    char* valor = hash_obtener(hash, "pepito");
    char* valor2 = hash_obtener(hash, "pepito2");
    char* valor3 = hash_obtener(hash, "pepito3");
    printf("dato del hash pepito es: %s \n",valor);
    printf("dato del hash pepito2 es: %s \n",valor2);
    printf("dato del hash pepito3 es: %s \n",valor3);
    if (hash_pertenece(hash,"pepito")){
        printf("pepito pertenece al set\n");
    }
    if (hash_pertenece(hash,"pepito2")){
        printf("pepito2 pertenece al set\n");
    }
    printf("destruyo hash\n");
    hash_destruir(hash);
*/
    printf("Ahora pruebo guardando muchos nombres\n");
	size_t capacidad = 0;
    hash_t* nuevo_hash = hash_crear(destruir_clave_valor);
	char *linea_stdin = NULL;
	char *puntero_stdin = NULL;
	printf("argv[1] es %s\n", argv[1]);
    char const* fileName = argv[1]; /* should check that argc > 1 */
    FILE* file;
    if (argc==2) printf("hola\n");
    file = fopen(fileName, "r+"); /* should check the result */
    if (!file) {
    	printf("error reading the file %s!\n", argv[1]);
    }
    size_t i = 0;
	while( getline( &linea_stdin, &capacidad, file) != -1 ) {
	    i++;
	    if (i == 97) {
	    	printf ("97\n");
	    } else if (i == 197) {
	    	printf ("197\n");
	    } else if (i == 397) {
	    	printf ("397\n");
	    }
		puntero_stdin = strchr(linea_stdin, '\n');
		if (!puntero_stdin) {
	    	printf ("ERROR linea 101\n");
		} else {
			*puntero_stdin = '\0';
		}
		puntero_stdin = strchr(linea_stdin, ',');
		if (!puntero_stdin) {
	    	printf ("ERROR linea 107\n");
		} else {
			*puntero_stdin = '\0';
		}
		puntero_stdin++;
	    hash_guardar(nuevo_hash, linea_stdin, puntero_stdin);
	}
	fclose(file);
	printf("cantidad de datos guardados en hash: %zu\n", hash_cantidad(nuevo_hash));
	printf("tamanio del hash: %zu\n", nuevo_hash->tamanio);

	printf("ingrese consultas de nombres de una sola palabra, para salir presione enter dos veces:\n");
	while ( getline( &linea_stdin,&capacidad,stdin)) {
		if (!hash_pertenece(nuevo_hash, linea_stdin)) {
			printf("el nombre %s no existe en la base de datos\n",linea_stdin);
		} else {
			printf("el nombre %s, tiene por fecha de inscripción: %s\n",linea_stdin, (char*)hash_obtener(nuevo_hash, linea_stdin));
		}
		free(linea_stdin);
		printf("\n");
		printf("ingrese consultas de nombres de una sola palabra, para salir presione enter dos veces:\n");
	}
	hash_destruir(nuevo_hash);

	return 0;
}
