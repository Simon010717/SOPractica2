#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <ncurses.h>
#include "general.h"
#include "uno.h"
#include "dos.h"
#include "tres.h"
#include "cuatro.h"

#define PORT 3535

void keyToContinue()
{                                                          // Funcion que espera a que el usario presione cualquier tecla para continuar
    printw("\nPulse cualquier tecla para continuar...\n"); // Imprime el mensaje
    noecho();                                              // Impide que se muestre el caracter en consola.
    getch();                                               // Espera y obtiene el caracter.
    echo();                                                // Permite de nuevo el ingreso de caracteres.
    clear();                                               // Limpia la consola
}

int main(int argc, char *argv[])
{
    int clientfd, r, q;
    struct sockaddr_in client;
    //struct hostent *he;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0)
    {
        perror("\n-->error socket() client:");
        exit(-1);
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);

    inet_aton(argv[1], &client.sin_addr);
    bzero(client.sin_zero, 8);

    r = connect(clientfd, (struct sockaddr *)&client, (socklen_t)sizeof(struct sockaddr));
    if (r < 0)
    {
        perror("\n-->error connect() client: ");
        exit(-1);
    }

    int w = 1;
    
    initscr(); // Se incia la consola
    clear();   // Se limpia la consola
    refresh(); // Se actualiza la consola
    
    while (w)
    { //Ciclo continuo del menú

        int opcion, r; // Declaracion de las variables

        printw("------------Veterinaria Juano's------------\n\n"); // Impresion del menú
        printw("Opciones: \n");
        printw("    1. Ingresar Registro\n");
        printw("    2. Ver Registro\n");
        printw("    3. Eliminar Registro\n");
        printw("    4. Buscar Registro\n");
        printw("    5. Salir\n");
        printw("\n\n");
        printw("Digite su opción: ");

        scanw("%i", &opcion); // Lee la opcion ingresada
        r = send(clientfd,(void*)&opcion, sizeof(int),0);
        if(r<0){
            perror("error send client");
            exit(-1);
        }
        switch (opcion) // Seleccion de la funcion según la opcion ingresada
        {
        case 1:
            ingresarRegistro(clientfd); // Ejecuta la función de Ingreso que se encuentra en SourceCode/uno.c
            
            r = recv(clientfd, (void*)&q, sizeof(int), 0);

            if (q != 0)
            {
                printw("Registro fallido.\n");
            }
            else
            {
                printw("Registro exitoso.\n");
            }
            break;

        case 2:
            verRegistro(clientfd); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 3:
            eliminarRegistro(clientfd); // Ejecuta la función de eliminarRegistro que se encuentra en SourceCode/tres.c
            r = recv(clientfd, (void*)&q, sizeof(int), 0);
            if(q) printw("Registro eliminado exitosamente\n"); 
            break;

        case 4:
            buscarRegistro(clientfd); // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 5:
            printw("Gracias.\n"); //Imprime gracias y termina el programa.
            w = 0;
            break;

        default:
            printw("Opcion invalida.\n"); //Imprime el mensaje.
        }

        keyToContinue();
    }
    clear();
    endwin();
    close(clientfd);
    return 0;
}