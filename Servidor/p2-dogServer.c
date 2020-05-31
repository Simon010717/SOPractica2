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

/*
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};
*/


/*
void interrumpido(){
    printw("señal interrumpida");
    close(serverfd);
}*/

struct arguments{
    int i;
    int clientfd;
    char ip[10];
};

void* client_manager(void* ar){
    struct arguments *args;
    args = ar;

    int i = args->i;
    int clientfd = args->clientfd;
    char* ip = args->ip;
    //printw("client %i %s",args->clientfd,args->ip);
    //printw("client %i %s",clientfd,ip);
    /*fp = fopen("client_manager","a");
    fwrite(men,strlen(men),1,fp);
    fwrite(buf,19,1,fp);
    fclose(fp);*/

    int r, opcion,q,w = 1;

    while (w) {

        r = recv(clientfd, (void *)&opcion, sizeof(int), 0);
        if(r<0){
            perror("error receive server");
            exit(-1);
        }
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
            //i--;
            break;
        }
    }
}

int main()
{
    //signal(SIGINT, interrumpido);

    int clientfd[32];
    int serverfd, r, q, opcion, opt = 1;
    char *ip;
    struct sockaddr_in server, clients[32];
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

    setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if (r < 0)
    {
        perror("\n-->Error en bind(): ");
        exit(-1);
    }

    r = listen(serverfd,BACKLOG);
    if (r < 0)
    {
        perror("\n-->error listen() server: ");
        exit(-1);
    }

    pthread_t tid[32];
    int i=0;

    while(1){
        clientfd[i] = accept(serverfd,(struct sockaddr*)&clients[i],&tamano);
        if (clientfd[i] < 0)
        {
            perror(" error accept() server: ");
            exit(-1);
        }
    
        ip = inet_ntoa(clients[i].sin_addr);
    
        struct arguments *args;
        args = malloc(sizeof(struct arguments));
        
        args->i=i;
        args->clientfd = clientfd[i];
        sprintf(args->ip,"%s",ip);

        if(pthread_create(&tid[i],NULL,client_manager,args) != 0){
            perror(" error thread no creado ");
            exit -1;
        }

        if(i>=32){
            i = 0;
            while(i<32){
                pthread_join(tid[i++],NULL);
            }
        }
        i++;
    }
}