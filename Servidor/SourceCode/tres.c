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
   |
   |  Retorna:  0 si la función es exitosa.
   -------------------------------------------------------------------/*/

int eliminarRegistro(int clientfd, char* ip){
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
    strcat(log," borrado ");
    int total, id, f, tid, hash, size, r;                    // Declaración de las variables enteras

    FILE *fp;                                             // Inicializacion del apuntador al archivo dataDogs.dat
    FILE *newfp;                                          // Inicializacion del apuntador a una copia del archivo dataDogs.dat omitiendo el registro a eliminar.
    fp = fopen("./dataDogs.dat","a");                     // Apertura de dataDogs.dat con argumento "a" para escribir al final del fichero
    total = ftell(fp)/100; 

    r = send(clientfd,(void*)&total,sizeof(int),0);

    r = recv(clientfd,(void*)&id,sizeof(int),0);
    
    hash = id % 1000;                                     // Guardamos en la variable hash el codigo hash de la estructura aplicandole modulo mil a el id

    char *dir;                                            // Arreglo de caracteres que contendra la direccion del archivo de la tabla hash.
    dir = malloc(25);                                     // Reserva del espacio de memoria para la direccion
    bzero(dir,25);                                        // Limpieza del espacio de memoria de la direccion                                                    
    char num[3];                                          // Arreglo de caracteres que contiene el codigo hash de la estructura
    strcat(dir,"./hash/");                                // Concatenacion de la cadena "./hash/" a dir
    sprintf(num,"%i",hash);                               // Conversion del numero entero hash a arreglo de caracteres (num)
    strcat(dir,num);                                      // Concatenacion del numero (num) a dir. Ahora dir contiene la direccion del archivo hash correspondiente

    fp = fopen(dir,"a");                                  // Apertura de dataDogs.dat con argumento "a" para escribir al final del fichero.
    size = ftell(fp)/36;                                  // Se guarda en la variable size el numero de elementos en la tabla hash.
    fclose(fp);                                           // Cierra el archivo dataDogs.dat

    fp = fopen(dir,"r");                                  // Apertura del archivo hash "r" (para lectura).
    newfp = fopen("./hash/temp","a");                     // Creacion de la copia del archivo hash (Vacio) con argumento "a" para escribir al final del fichero.

    char *m = malloc(36);                                 // Reserva del espacio de memoria para el paquete (id,nombre) que copiamos al segundo archivo

    f = 0;
    for ( int i = 0; i < size; i++ ){                     // Itera sobre los paquetes almacenados en el archivo hash
        fread(&tid,sizeof(int),1,fp);                     // Lee la id del paquete en iteracion y la guarda en la variable tid
        if(tid==id){                                      // Evalua si el tid es igual a el id ingresado
            fseek(fp,36-sizeof(int),SEEK_CUR);            // Salta a la siguiente estructura en el archivo omitinedo la que se busca eliminar
            f = 1;                                        // Cambia el valor de f a 1, indicando que se encontro un paquete con el id ingresado

        }
        else{                                             // Si no coincide
            fseek(fp,-sizeof(int),SEEK_CUR);              // Devuelve el apuntador de lectura de fp 4 bytes (Al inicio del paquete actual)
            fread(m, 36, 1, fp);                          // Lee y guarda en m el paquete actual
            fwrite(m, 36, 1, newfp);                      // Escribe sobre el segundo archivo el paquete leido.
        }
    }

    fclose(fp);                                           // Cierra el archivo hash
    fclose(newfp);                                        // Cierra la copia del archivo hash


    remove(dir);                                          // Borra el archivo hash original
    rename("./hash/temp", dir);                           // Renombra el segundo archivo a el nombre de el archivo original 
    
    r = send(clientfd, (void*)&f, sizeof(int),0);
    if(!f) {                                              // Evalua
        return 0;                                         // Termina la funcion
    }
    bzero(&tid,sizeof(int));                              // Limpieza del espacio de memoria del tid
    fp = fopen("./dataDogs.dat","r");                     // Apertura de dataDogs.dat con argumento "r" para lectura.     
    newfp = fopen("./newdata.dat","a");                   // Creacion de newData.dat (copia de dataDogs omitiendo la estructura a eliminar) con
    
    char *c = malloc(100);                                // Reserva del espacio de memoria para la estructura que copiamos al segundo archivo
    

    for ( int i = 0; i < total; i++ ){                    // Iteracion sobre las estructuras del archivo
        fread(&tid,sizeof(int),1,fp);                     // Lee la id de la estructura en iteracion y la guarda en la variable tid
        if(tid==id){                                      // Evalua si el tid es igual a el id ingresado
            fseek(fp,100-sizeof(int),SEEK_CUR);           // Salta a la siguiente estructura en el archivo omitinedo la que se busca eliminar
        }
        else{                                             // Si no coincide
            fseek(fp,-sizeof(int),SEEK_CUR);              // Devuelve el apuntador de lectura de fp 4 bytes (Al inicio de la estructura actual actual)
            fread(c, 100, 1, fp);                         // Lee y guarda en c la estructura actual
            fwrite(c, 100, 1, newfp);                     // Escribe sobre el segundo archivo el paquete leido.
        }
    }

    fclose(fp);                                           // Cierre del archivo dataDogs.dat
    fclose(newfp);                                        // Cierre del archivo newData.dat

    bzero(dir,25);                                        // Limpieza del apuntador dir
    strcat(dir,"./historias/");                           // Concatenacion del prefijo que conteiene la ruta a la carpeta de historias clinicas
    char num2[10];                                        // Declaracion de la variable num2 que contendra el id del registro.
    sprintf(num2,"%i",id);                                // Conversion del entero a arreglo de caracteres
    strcat(dir,num2);                                     // Concatenacion de num2 (entero id) a la direccion
    strcat(dir,".txt");                                   // Concatenacion del sufijo .txt a la ruta del archivo
    remove(dir);                                          // Eliminacion del archivo de historia clinica (si existe)

    free(c);                                              // Se libera el espacio de memoria reservado para c
    free(m);
    free(dir);                                              // Se libera el espacio de memoria reservado para m
    
    remove("./dataDogs.dat");                             // Eliminacion del archivo original dataDogs.dat
    rename("./newdata.dat", "./dataDogs.dat");            // Renombra el segundo archivo a el nombre de el archivo original 
    
    sprintf(num,"%i",id);
    strcat(log,num);
    strcat(log,"\n");
    fp = fopen("serverDogs.log", "a"); // Apertura del archivo hash "a" (para escritura al final del archivo)
    fwrite(log,strlen(log),1,fp);
    fclose(fp);           // Cierre del archivo

    return 0;
}

