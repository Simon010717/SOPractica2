#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include "general.h"
#include "uno.h"
#include "dos.h"
#include "tres.h"
#include "cuatro.h"

#define PORT 3535
#define BACKLOG 32

struct arguments{ // Estructura que contiene los argumentos necesarios para el hilo.
    int i;
    int clientfd;
    char ip[10];
};

/*----------------- client_manager --------------------------------
   |  Funcion ingresarRegistro
   |
   |  Proposito: Función que se ocupa del clientes y es ejecutada por los hilos.
   |
   |  Parametros: 
   |    ar (IN) -- apuntador a la estructura que tiene los argumentos necesarios 
|                   para el buen funcionamiento del proceso.
   |
   |  Retorna:  
   |    void* (out) -- vacío
   -------------------------------------------------------------------/*/
void* client_manager(void* ar){
    struct arguments *args; // Declaración de la estructura.
    args = ar; //Casting del apuntador a la estructura.

    //Declaración de los argumentos de la estructura.
    int i = args->i;
    int clientfd = args->clientfd;
    char* ip = args->ip;
    
    int r, opcion,q,w = 1; //Declaración de vriables

    while (w) {//Ciclo continuo del menú en el cliente

        r = recv(clientfd, (void *)&opcion, sizeof(int), 0); //Recepción de la opción escogida por el usuario.
        if(r<0){
            perror("error receive server");//Verificación de error al recibir el resultado del servidor.
            exit(-1);
        }
        switch (opcion)
        {
        case 1:
            q = ingresarRegistro(clientfd,ip); // Ejecuta la función de Ingreso que se encuentra en SourceCode/uno.c
            r = send(clientfd, (void *)&q, sizeof(int), 0);// Envío al cliente del resultado de ejecutar la función.
            if(r<sizeof(int)){
                perror("error send server");//Verificación de error al enviar el resultado al cliente.
                exit(-1);
            }
            break;

        case 2:
            verRegistro(clientfd,ip); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 3:
            q = eliminarRegistro(clientfd,ip); // Ejecuta la función de eliminarRegistro que se encuentra en SourceCode/tres.c
            r = send(clientfd, (void *)&q, sizeof(int), 0); // Envío al cliente del resultado de ejecutar la función.
            if(r<sizeof(int)){
                perror("error send server");//Verificación de error al enviar el resultado al cliente.
                exit(-1);
            }
            break;

        case 4:
            buscarRegistro(clientfd,ip); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 5:
            w = 0;//Sale del ciclo al acabar iteración.
            close(clientfd);//Cierre de la conexión con el cliente.
            break;
        }
    }
}

int main()
{

    int clientfd[32]; //Arreglo que contiene los sockets de clientes.
    int serverfd, r, q, opcion, opt = 1; //Declaración de variables.
    char *ip; //Apuntador para guardar la ip que se le pasa al hilo que hace el proceso del log.
    struct sockaddr_in server, clients[32];//Declaración de la estructura que contiene la información de la conexxión del servidor y el arreglo de los clientes.
    socklen_t tamano; //Tamaño de la dirección.

    serverfd = socket(AF_INET, SOCK_STREAM, 0);//Declaración del socket del servidor.
    if (serverfd < 0)
    {
        perror("\n-->Error en socket():");//Verificación de error al declarar el socket del servidor.
        exit(-1);
    }

    server.sin_family = AF_INET;//Familia de la dirección
    server.sin_port = htons(PORT);//Asignación del puerto.
    server.sin_addr.s_addr = INADDR_ANY;//Se acota el socket entre las interfaces de red del host.
    bzero(server.sin_zero, 8);//Se llena de ceros la estructura.

    setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));//Se permite la reutilización del puerto.

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));//Asigna el valor addr a la dirección.
    if (r < 0)
    {
        perror("\n-->Error en bind(): ");//verificación de error al hacer bind();
        exit(-1);
    }

    r = listen(serverfd,BACKLOG);//Se marca el socket como disponible para recibir solicitudes entrantes de connexión de parte de los clientes,
    if (r < 0)
    {
        perror("\n-->error listen() server: ");//Verificación del error al hacer Listen.
        exit(-1);
    }

    pthread_t tid[32];//Estructura necesaria para crear un hilo.
    int i=0;//Variable que controla el ciclo.

    while(1){//Ciclo que realiza la conexión con los clientes continuamente.
        clientfd[i] = accept(serverfd,(struct sockaddr*)&clients[i],&tamano);// Ejecución de la aceptación de las solicitudes de cada cliente. 
        if (clientfd[i] < 0)
        {
            perror(" error accept() server: ");//Verificación de error al aceptar la conexión con los clientes.
            exit(-1);
        }
    
        ip = inet_ntoa(clients[i].sin_addr);//Manipulación de la dirección IPv4.
    
        struct arguments *args;//Declaración de la estructura de argumentos de los hilos.
        args = malloc(sizeof(struct arguments));// Reserva el espacio de memoria necesario para la estructura.
        
        args->i=i;// Asignación al atributo i de la estructura de argumentos de la variable que itera en el ciclo.
        args->clientfd = clientfd[i];// Guarda en la estructura de argumentos el cliente que ya fue aceptado.
        sprintf(args->ip,"%s",ip);//Guarda la ip del cliente aceptado en la estructura de argumentos.

        if(pthread_create(&tid[i],NULL,client_manager,args) != 0){//Si no hay error, se crea un nuevo hilo
            perror(" error thread no creado ");
            exit -1;
        }

        if(i>=32){// Se hace join de todos los hilos que manejan a los hilos, si el número de los clientes concurrentes es mayor a 32.
            i = 0;
            while(i<32){
                pthread_join(tid[i++],NULL);
            }
        }
        i++;// aumento a la varible que itera en el ciclo.
    }
}