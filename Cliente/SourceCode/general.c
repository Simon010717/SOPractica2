#include "general.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/*---------------------------------hashf -----------------------------
   |  Funcion hashf
   |
   |  Proposito: Retorna el codigo hash de un arreglo de caracteres
   |  (nombre de la mascota) con cardinalidad 1000. 
   |
   |  Parametros:
   |    word (IN) -- Arreglo de caracteres.
   |    l (IN) -- Longitud del arreglo de caracteres.
   |
   |  Retorna: Entero equivalete al codigo hash cardinalidad 1000.
   -------------------------------------------------------------------/*/
int hashf(char word[],int l){
    int m,h,i,x,p;                                    // Declaracion de enteros usados en la formula de calculo del codigo
    m = 1000;                                         // Declaracion de la cardinalidad de la tabla hash (valor maximo del codigo hash resultante)

    p = 1009;                                         // Valor primo p > m para el calculo del codigo hash
    x = 22;                                           // Valor 0 < x < 32 (longitud max.)para el calculo del cosigo hash 

    for(i=0;i<l;i++){                                 // Iteracion sobre los caracteres del arreglo
        word[i] = tolower(word[i]);                   // Conversion a minuscula de los caracteres del arreglo
    }

    h = 1;                                            // Inicializacion del valor hash en 1. 
    for(i=0; i<l; i++){                               // Iteracion sobre los caracteres del arreglo, pues la formula es una sumatoria de operaciones realizadas sobre los codigos ASCII de los caracteres
        h = (p + h*x+(int)word[i])%p;                 // Suma del calculo sobre el caracter modulo p
    }

    return h%m;                                       // Retorna el valor del calculo (sumatoria) modulo m (cardinalidad de la tabla), este es el codigo hash resultante
}