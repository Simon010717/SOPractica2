#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "general.h"

/*----------------- eliminarRegistro --------------------------------
   |  Funcion: eliminarRegistro
   |
   |  Proposito: Verificar si un registro existe, eliminarlo de dataDogs.dat y 
   |  de la entrada a la tabla hash disminuyendo el tamaño de los archivos. 
   |
   |  Parametros:
   |    clientfd (IN) -- Socket del servidor.
   |    ip (IN) -- Arreglo de caracteres que contiene la ip a imprimir
   |                 en el log.
   |
   |  Retorna:  0 si la función es exitosa.
   -------------------------------------------------------------------/*/

int eliminarRegistro(int clientfd, char* ip){
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
    strcat(log," borrado ");// Une la cadena " borrado " a lo que hay en log.
    int total, id, f, tid, hash, size, r; // Declaración de las variables enteras

    FILE *fp; // Inicializacion del apuntador al archivo dataDogs.dat
    FILE *newfp; // Inicializacion del apuntador a una copia del archivo dataDogs.dat omitiendo el registro a eliminar.
    fp = fopen("./dataDogs.dat","a"); // Apertura de dataDogs.dat con argumento "a" para escribir al final del fichero
    total = ftell(fp)/100; // numero de registros totales.

    r = send(clientfd,(void*)&total,sizeof(int),0); // Envío del total de registros al cliente.
    if (r < sizeof(int))
    {
        perror("\n-->error send() total: "); //Verificación de error al enviar el total de registros al cliente.
        exit(-1);
    }

    r = recv(clientfd,(void*)&id,sizeof(int),0); // recepción del id enviado por el cliente.
    if (r < sizeof(int))
    {
        perror("\n-->error recv() id: "); //Verificación de error al recibir el id enviado por el cliente.
        exit(-1);
    }
    
    hash = id % 1000; // Guardamos en la variable hash el codigo hash de la estructura aplicandole modulo mil a el id

    char *dir; // Arreglo de caracteres que contendra la direccion del archivo de la tabla hash.
    dir = malloc(25); // Reserva del espacio de memoria para la direccion
    bzero(dir,25); // Limpieza del espacio de memoria de la direccion                                                    
    char num[3]; // Arreglo de caracteres que contiene el codigo hash de la estructura
    strcat(dir,"./hash/"); // Concatenacion de la cadena "./hash/" a dir
    sprintf(num,"%i",hash); // Conversion del numero entero hash a arreglo de caracteres (num)
    strcat(dir,num); // Concatenacion del numero (num) a dir. Ahora dir contiene la direccion del archivo hash correspondiente

    fp = fopen(dir,"a"); // Apertura de dataDogs.dat con argumento "a" para escribir al final del fichero.
    size = ftell(fp)/36; // Se guarda en la variable size el numero de elementos en la tabla hash.
    fclose(fp); // Cierra el archivo dataDogs.dat

    fp = fopen(dir,"r"); // Apertura del archivo hash "r" (para lectura).
    newfp = fopen("./hash/temp","a"); // Creacion de la copia del archivo hash (Vacio) con argumento "a" para escribir al final del fichero.

    char *m = malloc(36); // Reserva del espacio de memoria para el paquete (id,nombre) que copiamos al segundo archivo

    f = 0;
    for ( int i = 0; i < size; i++ ){ // Itera sobre los paquetes almacenados en el archivo hash
        fread(&tid,sizeof(int),1,fp); // Lee la id del paquete en iteracion y la guarda en la variable tid
        if(tid==id){ // Evalua si el tid es igual a el id ingresado
            fseek(fp,36-sizeof(int),SEEK_CUR); // Salta a la siguiente estructura en el archivo omitinedo la que se busca eliminar
            f = 1; // Cambia el valor de f a 1, indicando que se encontro un paquete con el id ingresado

        }
        else{ // Si no coincide
            fseek(fp,-sizeof(int),SEEK_CUR); // Devuelve el apuntador de lectura de fp 4 bytes (Al inicio del paquete actual)
            fread(m, 36, 1, fp); // Lee y guarda en m el paquete actual
            fwrite(m, 36, 1, newfp); // Escribe sobre el segundo archivo el paquete leido.
        }
    }

    fclose(fp); // Cierra el archivo hash
    fclose(newfp); // Cierra la copia del archivo hash


    remove(dir); // Borra el archivo hash original
    rename("./hash/temp", dir); // Renombra el segundo archivo a el nombre de el archivo original 
    
    r = send(clientfd, (void*)&f, sizeof(int),0);
    if (r < sizeof(int))
    {
        perror("\n-->error send() f: "); //Verificación de error al recibir el total de registros enviado por el servidor.
        exit(-1);
    }
    if(!f) { // Evalua
        return 0; // Termina la funcion
    }
    bzero(&tid,sizeof(int)); // Limpieza del espacio de memoria del tid
    fp = fopen("./dataDogs.dat","r"); // Apertura de dataDogs.dat con argumento "r" para lectura.     
    newfp = fopen("./newdata.dat","a"); // Creacion de newData.dat (copia de dataDogs omitiendo la estructura a eliminar) con
    
    char *c = malloc(100); // Reserva del espacio de memoria para la estructura que copiamos al segundo archivo
    

    for ( int i = 0; i < total; i++ ){ // Iteracion sobre las estructuras del archivo
        fread(&tid,sizeof(int),1,fp); // Lee la id de la estructura en iteracion y la guarda en la variable tid
        if(tid==id){ // Evalua si el tid es igual a el id ingresado
            fseek(fp,100-sizeof(int),SEEK_CUR); // Salta a la siguiente estructura en el archivo omitinedo la que se busca eliminar
        }
        else{ // Si no coincide
            fseek(fp,-sizeof(int),SEEK_CUR); // Devuelve el apuntador de lectura de fp 4 bytes (Al inicio de la estructura actual actual)
            fread(c, 100, 1, fp); // Lee y guarda en c la estructura actual
            fwrite(c, 100, 1, newfp); // Escribe sobre el segundo archivo el paquete leido.
        }
    }

    fclose(fp); // Cierre del archivo dataDogs.dat
    fclose(newfp); // Cierre del archivo newData.dat

    bzero(dir,25); // Limpieza del apuntador dir
    strcat(dir,"./historias/"); // Concatenacion del prefijo que conteiene la ruta a la carpeta de historias clinicas
    char num2[10]; // Declaracion de la variable num2 que contendra el id del registro.
    sprintf(num2,"%i",id); // Conversion del entero a arreglo de caracteres
    strcat(dir,num2); // Concatenacion de num2 (entero id) a la direccion
    strcat(dir,".txt"); // Concatenacion del sufijo .txt a la ruta del archivo
    remove(dir); // Eliminacion del archivo de historia clinica (si existe)

    free(c); // Se libera el espacio de memoria reservado para c
    free(m); //Se libera el espacio de memoria reservado para m
    free(dir); // Se libera el espacio de memoria reservado para dir
    
    remove("./dataDogs.dat"); // Eliminacion del archivo original dataDogs.dat
    rename("./newdata.dat", "./dataDogs.dat"); // Renombra el segundo archivo a el nombre de el archivo original 
    
    sprintf(num,"%i",id);// guarda en num lo que hay en id.
    strcat(log,num);// agrega num a lo que hay en log.
    strcat(log,"\n");// agrega un salto de line al final del log.
    fp = fopen("serverDogs.log", "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
    fwrite(log,strlen(log),1,fp);// escribe el log en el archivo correspondiente.
    fclose(fp); // Cierre del archivo

    return 0;
}

