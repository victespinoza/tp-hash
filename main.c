#include "testing.h"
#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

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

int main(){
    hash_t* hash = hash_crear(destruir_clave_valor);
    printf("Guardando hash pepito\n");
    hash_guardar(hash, "pepito","valor");
  /*  printf("Guardando hash pepito2\n");
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
    }*/
    printf("destruyo hash\n");
    hash_destruir(hash);
}
