#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/*----------------- verRegistro --------------------------------
   |  Funcion: verRegistro
   |
   |  Proposito: Muestra el registro correspondiente al id
   |  ingresado por el usuario
   |
   |  Parametros: 
   |
   |  Retorna:  0 si la operacion es exitosa.
   -------------------------------------------------------------------/*/
int verRegistro(int clientfd)
{
    int size, id, v, tid, r; // Declaracion de las variables enteras.                                                          // Cierre del archivo.

    r = recv(clientfd, (void *)&size, sizeof(int), 0);

    printf("Número de registros actuales: %i\nIngrese ID: ", size); // Impresion del numero de registros actuales y solicitud de ingreso del id del registro que se desea ver.

    scanf("%i", &id); // Lectura del id del registro que se desea ver.

    r = send(clientfd, (void *)&id, sizeof(int), 0);

    r = recv(clientfd, (void *)&v, sizeof(int), 0);
    if (!v)
    {                                                   // Si el id no fue encontrado
        printf("\nNo existe registro con ID %i\n", id); // Se informa al usuario
        return -1;                                      // La función retorna -1 (no exitoso)
    }

    struct dogType *data;                  // Declaracion de un apuntador de una estructura dogType.
    data = malloc(sizeof(struct dogType)); // Reserva del espacio de memora de la estructura dogType

    r = recv(clientfd, (void *)data, sizeof(struct dogType), 0); // Lectura del id y almacenamiento de este en la variable temporal tid.
    printf("\n\nRegistro  con el ID: %i\n", data->id);
    printf("Nombre: %s\n", data->nombre);
    printf("Tipo: %s\n", data->tipo);
    printf("Edad: %i\n", data->edad);
    printf("Raza: %s\n", data->raza);
    printf("Estatura: %i\n", data->estatura);
    printf("Peso: %f\n", data->peso);
    printf("Sexo: %c\n", data->sexo);

    char *dir;                   // Arreglo de caracteres que contendra la direccion del archivo de historia clinica.
    dir = malloc(15);            // Reserva del espacio de memoria para la direccion.
    bzero(dir, 15);              // Limpieza del espacio de memoria de la direccion.
    sprintf(dir,"%i",data->id);
    strcat(dir,".txt");

    FILE *fp;
    fp = fopen(dir,"a");
    r = recv(clientfd,(void*)&size,sizeof(int),0);
    if(size>0){
        char* buff;
        buff = malloc(size);
        bzero(buff,size);
        r = recv(clientfd,buff,size,0);
        fwrite(buff,size,1,fp);
        fclose(fp);
        free(buff);
    }

    
    char *com;             // Arreglo de caracteres que contendrá el comando para la apertura por consola de la historia clinica.
    com = malloc(35);      // Reserva del espacio de memoria para el comando.
    bzero(com, 35);        // Limpieza del espacio de memora para el comando.
    strcat(com, "gedit "); // Concatenacion de la cadena "gedit " a com.
    strcat(com, dir);      // Concatenacion de dir a com.

    system(com); // Ejecucion en consola del comando (com).
    
    fp = fopen(dir, "a"); // Creacion del archivo de historia clinica "a" (para escritura al final del archivo).
    size = ftell(fp);
    fclose(fp);           // Cierre del archivo.
    
    r = send(clientfd,(void*)&size,sizeof(int),0);
    if(size>0){
        char* buff;
        buff = malloc(size);
        bzero(buff,size);
        fp = fopen(dir, "r"); // Creacion del archivo de historia clinica "a" (para escritura al final del archivo).
        fread(buff,size,1,fp);
        fclose(fp);           // Cierre del archivo.
        r = send(clientfd,buff,size,0);
        free(buff);
    }

    remove(dir);


    free(data); // Se libera el espacio de memoria reservado para data                                                           // Se libera el espacio de memoria reservado para com
    free(dir);  // Se libera el espacio de memoria reservado para dir
    free(com);  // Se libera el espacio de memoria reservado para com
    
    return 0;
}