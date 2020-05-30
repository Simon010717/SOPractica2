#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <ncurses.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include "general.h"


/*----------------- buscarRegistro -------------------------------------
   |  Funcion: buscarRegistro
   |
   |  Proposito: Busca y muestra todos los registros que coinciden con   
   |  el nombre ingresado sin tener en cuenta la capitalizacion
   |
   |  Parametros:
   |
   |  Retorna:  0 si la función es exitosa.
   -------------------------------------------------------------------/*/

int buscarRegistro(int clientfd){
    int hash,size,tid,l,i,r;                                   // Declaracion de variables enteras
    char nombre[32];                                         // Declaracion del arreglo de caracteres del nombre buscado 
    bzero(nombre,32);                     // Limpieza del espacio en memoria 
    printf("Ingrese nombre a buscar: ");                     // Solicitud de ingreso de nombre 
    scanf("%s",nombre);                                      // Lectura del nombre ingresado

    r = send(clientfd,(void*)nombre, 32, 0);

    printf("ID de registros con nombre \"%s\":\n",nombre);   // Impresion de mensaje

    while(1){                              // Iteracion sobre paquetes en el archivo hash. Todos los archivos con el mismo nombre se encuentran en el mismo archivo hash, pero dos nombres diferentes pueden tener el mismo codigo hash
        r = recv(clientfd,(void*)&tid,sizeof(int),0);
        if(tid<0) break;
        printf("- %i ",tid);                             // Si coinciden imprime el id leido 
    }
    printf("\n");
    return 0;
}