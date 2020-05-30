#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#include "general.h"

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
int ingresarRegistro(int clientfd, char* ip)
{
   char log[100];
   char date[21];
   time_t tnow;
	struct tm *now;
	tnow = time(NULL);
	now = localtime(&tnow);
   bzero(log,100);
   bzero(date,21);
	sprintf(date,"%i 0%i %i %i:%i:%i ",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
   strcat(log,date);
   strcat(log,"Cliente ");
   strcat(log,ip);
   strcat(log," inserción ");
   int r, pos, id, hash, size;            // Declaracion de variables enteros
   int *tempid;                           // Declaracion del apuntador donde se guardara el id leido en la interacion de los archivos
   struct dogType *data;                  // Declaracion del apuntador a una estructura dogType donde se guardara la informacion ingresada
   data = malloc(sizeof(struct dogType)); // Reserva del espacio de memoria de la estuctura dogType

   r = recv(clientfd, (void *)data, sizeof(struct dogType), 0); // Llamada a la funcion que cargara la informacion ingresada al apuntador data
   
   hash = hashf(data->nombre, 32); // Llamada a la funcion hashf obteniendo el coidgo hash del nombre ingresado, que se encuentra en SourceCode/general.c

   char *dir;                // Arreglo de caracteres que contendra la direccion del archivo de la tabla hash donde se debera ingresar los datos
   dir = malloc(15);         // Reserva del espacio de memoria para la direccion
   bzero(dir, 15);           // Limpieza del espacio de memoria de la direccion
   char num[3];              // Arreglo de caracteres que contiene el codigo hash de la estructura
   strcat(dir, "./hash/");   // Concatenacion de la cadena "./hash/" a dir
   sprintf(num, "%i", hash); // Conversion del numero entero hash a arreglo de caracteres (num)
   strcat(dir, num);         // Concatenacion del numero (num) a dir. Ahora dir contiene la direccion del archivo hash correspondiente

   FILE *fp;             // Declaracion del apuntador del archivo de la entrada a la tabla hash
   fp = fopen(dir, "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
   size = ftell(fp);     // Se guarda en la variable size la posicion en Bytes del apuntador interno del archivo. Esto nos permite saber el tamaño del archivo en Bytes.
   fclose(fp);           // Cierre del archivo

   id = 1000 * (int)(size / 36) + hash; // Sabiendo el tamaño del archivo de esa entrada hash especifica, podemos calcular un id unico almaceando en la variable id
   data->id = id;                       // Se almacena este id en la estructura dogType donde ya se ha almecenado los datos ingresados

   fp = fopen(dir, "a");            // Apertura del documento hash con argumento "a" para escribir al final del fichero
   fwrite(&id, sizeof(int), 1, fp); // Escritura del id de la estructura
   fwrite(data->nombre, 32, 1, fp); // Escritura del nombre ingresado
   fclose(fp);                                      // Cierre del archivo
   
   fp = fopen("dataDogs.dat", "a");                 // Apertura del archivo dataDogs.dat con argumento "a" para esciribr al final del fichero
   r = fwrite(data, sizeof(struct dogType), 1, fp); // Escritura al final del archivo de la estructura entera con todos los datos ingresados
   fclose(fp);                                      // Cierre del archivo

   free(data);                                     // Liberacion de la memoria reservada en data (estructura donde se guardaron los datos)
   free(dir);                                      // Liberacion de la memoria reservada en dir (arreglo que contenia la direccion del archivo hash)
   r = send(clientfd, (void *)&id, sizeof(int), 0); //Envio del id resultado del ingreso del registro
   
   sprintf(num,"%i",id);
   strcat(log,num);
   strcat(log,"\n");
   fp = fopen("serverDogs.log", "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
   fwrite(log,strlen(log),1,fp);
   fclose(fp);           // Cierre del archivo

   return 0;
}