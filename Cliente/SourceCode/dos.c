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
#include <ncurses.h>
#include <signal.h>
#include "general.h"

/*----------------- verRegistro --------------------------------
   |  Funcion: verRegistro
   |
   |  Proposito: Muestra el registro correspondiente al id
   |  ingresado por el usuario
   |
   |  Parametros: 
   |     clientfd (IN) -- Socket del servidor.
   |
   |  Retorna:  0 si la operacion es exitosa.
   -------------------------------------------------------------------/*/
int verRegistro(int clientfd)
{
    int size, id, v, tid, r; // Declaracion de las variables enteras.

    r = recv(clientfd, (void *)&size, sizeof(int), 0); // recepción del tamaño de la estructura enviado por el servidor.
    if (r < sizeof(int))
    {
        perror("\n-->error recv() size: "); //Verificación de error al recibir el tamaño de la estructura enviado por el servidor.
        exit(-1);
    }

    printw("Número de registros actuales: %i\nIngrese ID: ", size); // Impresion del numero de registros actuales y solicitud de ingreso del id del registro que se desea ver.

    scanw("%i", &id); // Lectura del id del registro que se desea ver.

    r = send(clientfd, (void *)&id, sizeof(int), 0); // envío del id al servidor.
    if (r <  sizeof(int))
    {
        perror("\n-->error send() id: "); //Verificación de error al enviar el id al servidor.
        exit(-1);
    }

    r = recv(clientfd, (void *)&v, sizeof(int), 0); // recibe v, enviada por el servidor.
    if (r <  sizeof(int))
    {
        perror("\n-->error recv() v: "); //Verificación de error al enviar el id al servidor.
        exit(-1);
    }
    if (!v)
    { // Si el id no fue encontrado
        printw("\nNo existe registro con ID %i\n", id); // Se informa al usuario
        return -1; // La función retorna -1 (no exitoso)
    }

    struct dogType *data; // Declaracion de un apuntador de una estructura dogType.
    data = malloc(sizeof(struct dogType)); // Reserva del espacio de memora de la estructura dogType

    r = recv(clientfd, (void *)data, sizeof(struct dogType), 0); //recepción de la estructura enviada por el servidor.
    if (r <  sizeof(struct dogType))
    {
        perror("\n-->error recv() struct: "); //Verificación de error al enviar el id al servidor.
        exit(-1);
    }
    printw("\n\nRegistro  con el ID: %i\n", data->id); // impresión de los datos de la estructura.
    printw("Nombre: %s\n", data->nombre);
    printw("Tipo: %s\n", data->tipo);
    printw("Edad: %i\n", data->edad);
    printw("Raza: %s\n", data->raza);
    printw("Estatura: %i\n", data->estatura);
    printw("Peso: %f\n", data->peso);
    printw("Sexo: %c\n", data->sexo);

    char *dir; // Arreglo de caracteres que contendra la direccion del archivo de historia clinica.
    dir = malloc(15); // Reserva del espacio de memoria para la direccion.
    bzero(dir, 15); // Limpieza del espacio de memoria de la direccion.
    sprintf(dir,"%i",data->id); // asignación del id a dir.
    strcat(dir,".txt"); // se una la cadena ".txt" a dir.

    FILE *fp; // Inicializacion del apuntador al archivo de la historia.
    fp = fopen(dir,"a"); // apertura del archivo como 'a', solo lectura.
    r = recv(clientfd,(void*)&size,sizeof(int),0); // recepción del tamaño de la historia enviada por el servidor.
    if (r <  sizeof(int))
    {
        perror("\n-->error recv() size: "); //Verificación de error al recibir la historia enviada por el servidor.
    }
    if(size>0){// si la historia tiene tamaño mayor a cero, se envía al cliente.
        char* buff;// Variable con el contenido del archivo.
        buff = malloc(size);// reserva de memoria para el buffer.
        bzero(buff,size);// Se llena de ceros el buffer.
        r = recv(clientfd,buff,size,0); // recepcion del contenido de la historia enviada por el servidor.
        if (r < size)
        {
            perror("\n-->error recv() historia: "); //Verificación de error al recibir la historia enviada por el servidor.
            exit(-1);
        }
        fwrite(buff,size,1,fp); // escribe el contenido de la historia editada por el cliente en el archivo de historia clinica.
        fclose(fp);// cierra el la historia.
        free(buff); // libera el buffer.
    }

    
    char *com; // Arreglo de caracteres que contendrá el comando para la apertura por consola de la historia clinica.
    com = malloc(35); // Reserva del espacio de memoria para el comando.
    bzero(com, 35); // Limpieza del espacio de memora para el comando.
    strcat(com, "gedit "); // Concatenacion de la cadena "gedit " a com.
    strcat(com, dir); // Concatenacion de dir a com.

    system(com); // Ejecucion en consola del comando (com).
    
    fp = fopen(dir, "a"); // Creacion del archivo de historia clinica "a" (para escritura al final del archivo).
    size = ftell(fp);
    fclose(fp); // Cierre del archivo.
    
    r = send(clientfd,(void*)&size,sizeof(int),0); // envío del tamaño de la historia al servidor.
    if (r < sizeof(int))
    {
        perror("\n-->error send() size: "); //Verificación de error al enviar el tamaño de la historia al servidor.
        exit(-1);
    }
    if(size>0){
        char* buff;// Variable con el contenido del archivo.
        buff = malloc(size);// reserva de memoria para el buffer.
        bzero(buff,size);// Se llena de ceros el buffer.
        fp = fopen(dir, "r"); // Creacion del archivo de historia clinica "r" (solo lectura).
        fread(buff,size,1,fp); // almacena lo que hay en el archivo en la variable buff.
        fclose(fp);           // Cierre del archivo.
        r = send(clientfd,buff,size,0); // envío del contenido de la historia al servidor.
        if (r < size)
        {
            perror("\n-->error send() historia: "); //Verificación de error al enviar el contenido de la historia al servidor.
            exit(-1);
        }
        free(buff); // se libera la memoria reservada para buff.
    }

    remove(dir); // se remueve el archivo temporal que guardaba la historia.


    free(data); // Se libera el espacio de memoria reservado para data                                                           // Se libera el espacio de memoria reservado para com
    free(dir);  // Se libera el espacio de memoria reservado para dir
    free(com);  // Se libera el espacio de memoria reservado para com
    
    return 0;
}