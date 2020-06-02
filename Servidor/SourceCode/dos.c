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
#include <time.h>
#include "general.h"

/*----------------- verificacion --------------------------------
   |  Funcion: verificacion
   |
   |  Proposito: Verifica la existencia de una estructura
   |  almacenada que tiene el id ingresado por el usuario.
   |
   |  Parametros:
   |    id (IN) -- id de la estructura a comprobar
   |
   |  Retorna:  f = 1 si existe una estructura con el id ingresado.
   |            f = 0 si no existe una estructura con el id ingresado.
   -------------------------------------------------------------------/*/
int verificacion(int id)
{
    FILE *fp; // Inicializacion del apuntador al archivo hash donde se hara la verificacion.

    int f, tid, hash, size; // Declaracion de las variables enteras.

    hash = id % 1000; // Obtencion del codigo hash utilizando el id con la operacion id % 1000.

    char *dir;                // Arreglo de caracteres que contendra la direccion del archivo de la tabla hash.
    dir = malloc(15);         // Reserva del espacio de memoria para la direccion
    bzero(dir, 15);           // Limpieza del espacio de memoria de la direccion
    char num[3];              // Arreglo de caracteres que contiene el codigo hash de la estructura
    strcat(dir, "./hash/");   // Concatenacion de la cadena "./hash/" a dir
    sprintf(num, "%i", hash); // Conversion del numero entero hash a arreglo de caracteres (num)
    strcat(dir, num);         // Concatenacion del numero (num) a dir. Ahora dir contiene la direccion del archivo hash correspondiente

    fp = fopen(dir, "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
    size = ftell(fp);     // Se guarda en la variable size la posicion en Bytes del apuntador interno del archivo. Esto nos permite saber el tamaño del archivo en Bytes.
    fclose(fp);           // Cierre del archivo

    fp = fopen(dir, "r"); // Apertura del archivo hash "r" (para lectura).
    f = 0;
    for (int i = 0; i < size; i++)
    {                                    // Iteracion sobre los paquetes alamacenados en el archivo hash (un paquete contiene la id y el nombre de una estructura almacenada).
        fread(&tid, sizeof(int), 1, fp); // Lectura del id y almacenamiento de este en la variable temporal tid.
        if (tid == id)
        {          // Se verifica si la id leida (tid) corresponde a la id que se busca.
            f = 1; // f (encontrado) = 1 (verdadero)
            break; // Se sale del ciclo
        }
        else
        {                                          // El id leido (tid) no corresponde con el id que busca
            fseek(fp, 36 - sizeof(int), SEEK_CUR); // Salta al inicio del siguiente paquete para iniciar nuevamente con la verificación
        }
    }

    fclose(fp); // Cierre del archivo.
    free(dir);  // Se libera el espacio de memoria reservado para dir
    return f;   // Se retorna f (encontrado/no encontrado)
}

/*----------------- verRegistro --------------------------------
   |  Funcion: verRegistro
   |
   |  Proposito: Muestra el registro correspondiente al id
   |  ingresado por el usuario
   |
   |  Parametros: 
   |    clientfd (IN) -- Socket del servidor.
   |    ip (IN) -- Arreglo de caracteres que contiene la ip a imprimir
   |                 en el log.
   |
   |  Retorna:  0 si la operacion es exitosa.
   -------------------------------------------------------------------/*/
int verRegistro(int clientfd, char* ip)
{   
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
    strcat(log," lectura "); // Une la cadena " lectura " a lo que hay en log.
    int size, id, v, tid, r;           // Declaracion de las variables enteras.
    FILE *fp;                          // Inicializacion del apuntador al archivo dataDogs.dat.
    fp = fopen("./dataDogs.dat", "a"); // Apertura del archivo "a" (para escritura al final del archivo).
    size = (int)(ftell(fp) / 100);     // Se guarda en la variable size la posicion en Bytes del apuntador interno del archivo. Esto nos permite saber el numero de estructuras almacenadas en el archivo al dividir por el tamaño de una unica estructura.
    fclose(fp);                        // Cierre del archivo.

    r = send(clientfd, (void *)&size, sizeof(int), 0); // Envía la variable size al cliente.
    if (r < sizeof(int))
    {
        perror("\n-->error send() server: "); //Verificación de error al enviar data al cliente.
        exit(-1);
    }

    r = recv(clientfd, (void *)&id, sizeof(int), 0); // Recibe
    if (r < sizeof(int))
    {
        perror("\n-->error recv() server: "); //Verificación de error al recibir el id enviado por el cliente.
        exit(-1);
    }

    v = verificacion(id); // Llamada a la función verificacion.
    r = send(clientfd, (void *)&v, sizeof(int), 0);
    if(!v) return -1; // verificación del error al enviar la respuesta de ejecutar la función verificación.
    
    fp = fopen("./dataDogs.dat", "r"); // Apertura del archivo dataDogs.dat "r" (para lectura)

    struct dogType *data;                  // Declaracion de un apuntador de una estructura dogType.
    data = malloc(sizeof(struct dogType)); // Reserva del espacio de memora de la estructura dogType

    for (int i = 0; i < size; i++)
    {                                    // Iteracion sobre las estructuras almacenadas y busqueda de la estructura con el id correspondiente en el archivo datadogs.dat
        fread(&tid, sizeof(int), 1, fp); // Lectura del id y almacenamiento de este en la variable temporal tid (iterante).
        if (tid == id)
        {
            data->id = id;                  // Se lee la informacion del archivo y se guarda en la estructura data.
            fread(data->nombre, 32, 1, fp); 
            fread(data->tipo, 32, 1, fp);
            fread(&data->edad, sizeof(int), 1, fp);
            fread(data->raza, 16, 1, fp);
            fread(&data->estatura, sizeof(int), 1, fp);
            fread(&data->peso, sizeof(double), 1, fp);
            fread(&data->sexo, 1, 1, fp);
            r = send(clientfd, (void *)data, sizeof(struct dogType), 0); // envío de la estructura data al cliente.
            if (r <  sizeof(struct dogType))
            {
                perror("\n-->error send() server: "); //Verificación de error al enviar data al cliente.
                exit(-1);
            }
            break;
        }
        else
        {                                           // Si no corresponde,
            fseek(fp, 100 - sizeof(int), SEEK_CUR); // Salta al inicio de la siguiente estructura
        }
    }

    char *dir; // Arreglo de caracteres que contendra la direccion del archivo de historia clinica.
    dir = malloc(25); // Reserva del espacio de memoria para la direccion.
    bzero(dir, 25); // Limpieza del espacio de memoria de la direccion.
    char num[8]; // Arreglo de caracteres que contendra el numero de historia clinica correspondiente.
    strcat(dir, "./historias/"); // Concatenacion de la cadena "./historias/" a dir.
    sprintf(num, "%i", id); // Conversion del numero entero id a arreglo de caracteres (num).
    strcat(dir, num); // Concatenacion del numero (num) a dir.
    strcat(dir, ".txt"); // Concatenacion de la cadena ".txt" a dir.  Ahora dir contiene la direccion del archivo de historia clinica correspondiente.

    fp = fopen(dir, "a"); // Creacion del archivo de historia clinica "a" (para escritura al final del archivo).
    size = ftell(fp);// se asigna el tamaño del archivo a la variable size.
    fclose(fp); // Cierre del archivo.

    r = send(clientfd,(void*)&size,sizeof(int),0); // envío del tamaño del archivo al cliente.
    if (r <  sizeof(struct dogType))
    {
        perror("\n-->error send() server: "); //Verificación de error al enviar el tamaño del archivo al cliente.
        exit(-1);
    }
    if(size>0){ // si la historia tiene tamaño mayor a cero, se envía al cliente.
        char* buff; // Variable con el contenido del archivo.
        buff = malloc(size); // reserva de memoria para el buffer.
        bzero(buff,size); // Se llena de ceros el buffer.
        fp = fopen(dir, "r"); // Creacion del archivo de historia clinica "r" (para lectura).
        fread(buff,size,1,fp); // Se guarda en el buffer lo que hay en la historia
        fclose(fp);           // Cierre del archivo.
        r = send(clientfd,buff,size,0); // Se envía el buffer que contiene la historia, al cliente.
        if (r <  size)
        {
            perror("\n-->error send() server: "); //Verificación de error al enviar la historia, al cliente.
            exit(-1);
        }
        free(buff); // se libera el buffer.
    }
    
    remove(dir); // se remueve la historia anterior.
    fp = fopen(dir,"a"); // Creacion del archivo de historia clinica "a" (para escritura al final del archivo).
    r = recv(clientfd,(void*)&size,sizeof(int),0); // recibe el tamañp de la historia editada por el cliente.
    if (r <  sizeof(int))
    {
        perror("\n-->error recv() server: "); //Verificación de error al recibir el tamaño de la historia editada por cliente.
        exit(-1);
    }
    if(size>0){ // si la historia tiene tamaño mayor a cero, se envía al cliente.
        char* buff; // Variable con el contenido del archivo.
        buff = malloc(size);// reserva de memoria para el buffer.
        bzero(buff,size); // Se llena de ceros el buffer.
        r = recv(clientfd,buff,size,0); // recibe el buffer (contenido de la historia) enviado por el cliente.
        if (r <  size)
        {
            perror("\n-->error recv() server: "); //Verificación de error al recibir el buffer (contenido de la historia) enviado por el cliente.
            exit(-1);
        }
        fwrite(buff,size,1,fp); // escribe el contenido de la historia editada por el cliente en el archivo de historia clinica.
        fclose(fp); // cierra el la historia.
        free(buff); // libera el buffer.
    }

    free(data); // Se libera el espacio de memoria reservado para data
    free(dir);  // Se libera el espacio de memoria reservado para dir

    sprintf(num,"%i",id); // guarda en num lo que hay en id.
    strcat(log,num);// agrega num a lo que hay en log.
    strcat(log,"\n");// agrega un salto de line al final del log.
    fp = fopen("serverDogs.log", "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
    fwrite(log,strlen(log),1,fp); // escribe el log en el archivo correspondiente.
    fclose(fp); // Cierre del archivo

    return 0;
}