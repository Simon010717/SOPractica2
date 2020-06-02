#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "general.h"


/*----------------- buscarRegistro -------------------------------------
   |  Funcion: buscarRegistro
   |
   |  Proposito: Busca y muestra todos los registros que coinciden con   
   |  el nombre ingresado sin tener en cuenta la capitalizacion
   |
   |  Parametros:
   |    clientfd (IN) -- Socket del servidor.
   |    ip (IN) -- Arreglo de caracteres que contiene la ip a imprimir
   |                 en el log.
   |
   |  Retorna:  0 si la función es exitosa.
   -------------------------------------------------------------------/*/

int buscarRegistro(int clientfd, char* ip){
    char log[100]; // Areglo que contiene la cadena que se imprime en el log.
    char date[21]; // Fecha a imprimir en el log.
    time_t tnow; // Variable que contiene el tiempo a imprimir en el log.
	struct tm *now; // Apuntador a la estructura donde se guardará la información de local time.
	tnow = time(NULL); // Se inicializa el tiempo en null.
	now = localtime(&tnow); // se inicializa el localtime.
    bzero(log,100); // Se llena la estructura log de ceros.
    bzero(date,21); // Se llena la estructura date de ceros.
    sprintf(date,"%i 0%i 0%i %i:%i:%i ",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec); // Asigna los valores correspondientes a la fecha para ser impresos en el log.
    strcat(log,date); // Une la fecha al arreglo del log.
    strcat(log,"Cliente "); // Une la cadena "Cliente " a lo que hay en log.
    strcat(log,ip); // Une la ip a lo que hay en log.
    strcat(log," búsqueda ");// Une la cadena " búsqueda " a lo que hay en log.
    int hash,size,tid,l,i,r; // Declaracion de variables enteras
    char tnombre[32]; // Declaracion del arreglo de caracteres temporal de la iteracion 
    char nombre[32]; // Declaracion del arreglo de caracteres del nombre buscado 
    bzero(tnombre,32); bzero(nombre,32); // Limpieza del espacio en memoria 
    r = recv(clientfd,(void*)nombre, 32, 0); // Lectura del nombre ingresado
    if (r < 32)
    {
        perror("\n-->error recv() server: "); //Verificación de error al recibir nombre.
        exit(-1);
    }

    for(i=0;i<32;i++){ // Iteracion sobre los caracteres del nombre
        nombre[i] = tolower(nombre[i]); // Conversion de los caracteres a minusculas
    }

    hash = hashf(nombre,32); // LLamada a la funcion de hashf
    
    char *dir; // Arreglo de caracteres que contendra la direccion del archivo de la tabla hash.
    dir = malloc(15); // Reserva del espacio de memoria para la direccion
    bzero(dir,15); // Limpieza del espacio de memoria de la direccion
    char num[3]; // Arreglo de caracteres que contiene el codigo hash de la estructura
    strcat(dir,"./hash/"); // Concatenacion de la cadena "./hash/" a dir
    sprintf(num,"%i",hash); // Conversion del numero entero hash a arreglo de caracteres (num)
    strcat(dir,num); // Concatenacion del numero (num) a dir. Ahora dir contiene la direccion del archivo hash correspondiente

    FILE *fp; // Inicializacion del apuntador al archivo hash donde se hara la verificacion.
    fp = fopen(dir,"a"); // Apertura del archivo hash con argumento "a" para escribir al final del fichero.
    size = (int)(ftell(fp)/36); // Guarda en la variable size la cantidad de paquetes en elarchivo hash                                
    fclose(fp); // Cierre del archivo hash

    fp = fopen(dir,"r"); // Apertura del archivo hash con argumento "r" para lectutra
    while(ftell(fp)<(size*36)){ // Iteracion sobre paquetes en el archivo hash. Todos los archivos con el mismo nombre se encuentran en el mismo archivo hash, pero dos nombres diferentes pueden tener el mismo codigo hash
        fread(&tid,4,1,fp); // Lectura y almacenamiento del id del paquete en la variable tid
        fread(tnombre,32,1,fp); // Lectura de almacenamiento del nombre del paquete
        for(i=0;i<32;i++){tnombre[i] = tolower(tnombre[i]);} // Iteracion sobre los caracteres del nombre y conversion a minuscula 
        if(strcmp(nombre,tnombre) == 0){ // Compara el nombre a buscar y el nombre leido
           r = send(clientfd,(void*)&tid,sizeof(int),0); // Envío del id temporal al cliente.
           if (r < sizeof(int))
            {
                perror("\n-->error send() server: "); //Verificación de error al enviar el id.
                exit(-1);
            }
        }
        fseek(fp,36-sizeof(int)-32,SEEK_CUR); // Salto al inicio del siguiente paquete
    }
    int done = -1; 
    r = send(clientfd,(void*)&done,sizeof(int),0);// envío de la variable done al cliente.
    if (r < sizeof(int))
    {
        perror("\n-->error send() server: "); //Verificación de error al enviar done al cliente.
        exit(-1);
    }

    free(dir); // Liberar memoria de dir
    
    strcat(log,nombre); // une nombre a lo que hay en log.
    strcat(log,"\n"); // agrega un salto de linea al final de log
    fp = fopen("serverDogs.log", "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
    fwrite(log,strlen(log),1,fp);// escribe el log en el archivo correspondiente.
    fclose(fp); // Cierre del archivo
    
    return 0;
}