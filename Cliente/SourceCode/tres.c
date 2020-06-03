#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>

/*----------------- eliminarRegistro --------------------------------
   |  Funcion: eliminarRegistro
   |
   |  Proposito: Verificar si un registro existe, eliminarlo de dataDogs.dat y 
   |  de la entrada a la tabla hash disminuyendo el tamaño de los archivos. 
   |
   |  Parametros:
   |     clientfd (IN) -- Socket del servidor.
   |
   |  Retorna:  0 si la función es exitosa.
   -------------------------------------------------------------------/*/

int eliminarRegistro(int clientfd){
    int total, id, f, tid, hash, size, r;                    // Declaración de las variables enteras

    r = recv(clientfd,(void*)&total,sizeof(int),0); // recepción del total de registros enviado por el servidor.
    if (r < sizeof(int))
    {
        perror("\n-->error recv() total: "); //Verificación de error al recibir el total de registros enviado por el servidor.
        exit(-1);
    }


    printw("Existen %i registros.\n",total); // Impresion del numero de registros
    printw("ID de la mascota que desea eliminar: "); // Impresion mensaje
    scanw("%i",&id); // Lectura del id del registro a eliminare.

    r = send(clientfd,(void*)&id,sizeof(int),0);
    if (r < sizeof(int))
    {
        perror("\n-->error send() id: "); //Verificación de error al recibir el total de registros enviado por el servidor.
        exit(-1);
    }

    r = recv(clientfd,(void*)&f,sizeof(int),0); // recepción de si se encontró el registro buscado.
    if (r < sizeof(int))
    {
        perror("\n-->error recv() f: "); //Verificación de error al recibir el total de registros enviado por el servidor.
        exit(-1);
    }
    if(!f) {                                              // Evalua
        printw("No existe un registro con ese ID\n");     // Si f es 0 (queriendo decir que no encontro un paquete con ese id), imprime el mensaje
        return 0;                                         // Termina la funcion
    }
    return 0;
}

