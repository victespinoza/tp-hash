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

int main(){
    hash_t* hash = hash_crear(NULL);
    hash_guardar(hash, "pepito","valor");
    hash_guardar(hash, "pepito2","valor2");
    hash_guardar(hash, "pepito3","valor3");
    hash_guardar(hash, "pepito","valor actualizado");
    hash_borrar(hash, "pepito2");
    char* valor = hash_obtener(hash, "pepito");
    char* valor2 = hash_obtener(hash, "pepito2");
    char* valor3 = hash_obtener(hash, "pepito3");
    printf("%s \n",valor);
    printf("%s \n",valor2);
    printf("%s \n",valor3);
    if (hash_pertenece(hash,"pepito2")){
        printf("pertenece");
    }
    hash_destruir(hash);
}