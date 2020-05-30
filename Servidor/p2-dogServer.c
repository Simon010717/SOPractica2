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
#include "general.h"
#include "uno.h"
#include "dos.h"
#include "tres.h"
#include "cuatro.h"

#define PORT 3535
#define BACKLOG 2

/*
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};
*/

int serverfd;
/*
void interrumpido(){
    printf("señal interrumpida");
    close(serverfd);
}*/

int main()
{
    //signal(SIGINT, interrumpido);

    int clientfd, r, q, opcion, opt = 1;
    struct sockaddr_in server, client;
    socklen_t tamano;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        perror("\n-->Error en socket():");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);

    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if (r < 0)
    {
        perror("\n-->Error en bind(): ");
        exit(-1);
    }

    r = listen(serverfd, BACKLOG);
    if (r < 0)
    {
        perror("\n-->error listen() server: ");
        exit(-1);
    }

    clientfd = accept(serverfd, (struct sockaddr *)&client, &tamano);
    if (clientfd < 0)
    {
        perror("\n-->error accept() server: ");
        exit(-1);
    }

    char* ip;
    ip = inet_ntoa(client.sin_addr);

    int w = 1;

    while (w) {

        r = recv(clientfd, (void *)&opcion, sizeof(int), 0);
        //printf("opcion recieved %i", opcion);
        switch (opcion)
        {
        case 1:
            q = ingresarRegistro(clientfd,ip); // Ejecuta la función de Ingreso que se encuentra en SourceCode/uno.c
            r = send(clientfd, (void *)&q, sizeof(int), 0);
            break;

        case 2:
            verRegistro(clientfd,ip); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 3:
            q = eliminarRegistro(clientfd,ip); // Ejecuta la función de eliminarRegistro que se encuentra en SourceCode/tres.c
            r = send(clientfd, (void *)&q, sizeof(int), 0);
            break;

        case 4:
            buscarRegistro(clientfd,ip); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 5:
            w = 0;
            close(clientfd);
            close(serverfd);
            break;
        }
    }

    //r = send(clientfd, "hola mundo", 10, 0);
    /*if (r < 0)
    {
        perror("\n-->Error en send(): ");
        exit(-1);
    }*/
    
}