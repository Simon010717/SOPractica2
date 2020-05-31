#include <stdio.h>
#include <stdlib.h>
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
#include <pthread.h>

#include "general.h"

/*----------------- loadStruct --------------------------------
   |  Funcion: loadStruct
   |
   |  Proposito: Carga los datos ingresados por el usuario a un apuntador dogType que se pasa como parametro. 
   |
   |  Parametros:
   |    ap (IN) -- Apuntador al lugar donde se quiere guardar la informacion leida.
   |
   |  Retorna:  0 si la operacion es exitosa.
   -------------------------------------------------------------------/*/
int loadStruct(void *ap){
   struct dogType *data;                                                      // Declaracion de la estructura sobre el apuntador parametro
   data = ap;
   printw("Nombre: ");                                                        // Lectura de cada dato y almacenamiento sobre la estructura declarada
   bzero(data->nombre,32);
   scanw(" %s", data->nombre);
   printw("Tipo: ");
   scanw(" %s", data->tipo);
   printw("Edad: ");
   scanw(" %i", &data->edad);
   printw("Raza: ");
   scanw(" %s", data->raza);
   printw("Estatura: ");
   scanw(" %i", &data->estatura);
   printw("Peso: ");
   scanw(" %f", &data->peso);
   printw("Sexo: ");
   scanw(" %c", &data->sexo);

   return 0;
}


/*----------------- ingresarRegistro --------------------------------
   |  Funcion ingresarRegistro
   |
   |  Proposito: Guarda la informacion ingresada por el usuario en el 
   |  archivo dataDogs.dat e ingresa los datos a la tabla hash (que se
   |  encuentra en la carpeta hash) 
   |
   |  Parametros:
   |
   |  Retorna:  0 si la operacion es exitosa.
   -------------------------------------------------------------------/*/
int ingresarRegistro(int clientfd){
   int r,pos,id,hash,size;                                                   // Declaracion de variables enteros
   int *tempid;                                                              // Declaracion del apuntador donde se guardara el id leido en la interacion de los archivos
   
   struct dogType *data;                                                     // Declaracion del apuntador a una estructura dogType donde se guardara la informacion ingresada
   data = malloc(sizeof(struct dogType));                                    // Reserva del espacio de memoria de la estuctura dogType
   
   r = loadStruct(data);             // Llamada a la funcion que cargara la informacion ingresada al apuntador data
   r = send(clientfd, (void*)data, sizeof(struct dogType), 0);
   r = recv(clientfd, (void *)&id, sizeof(int),0);
   printw("ID: %i. ",id);                                                  // Impresion del ID con el que se guardo la informacion ingresado
   return 0;
}