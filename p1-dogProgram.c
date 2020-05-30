/*
 * Autores: Simon Aparicio, Juan Gonzalez, Samuel Gonzalez
 * Lenguaje:  C
 */

#include <stdio.h>
#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "general.h"
#include "uno.h"
#include "dos.h"
#include "tres.h"
#include "cuatro.h"



void keyToContinue(){                                                             // Funcion que espera a que el usario presione cualquier tecla para continuar
    printw("\nPulse cualquier tecla para continuar...\n");                        // Imprime el mensaje
    noecho();                                                                     // Impide que se muestre el caracter en consola.
    getch();                                                                      // Espera y obtiene el caracter.
    echo();                                                                       // Permite de nuevo el ingreso de caracteres.    
    clear();                                                                      // Limpia la consola
}


int main(){

    int w = 1;
    initscr();                                                                   // Se incia la consola 
    clear();                                                                     // Se limpia la consola
    refresh();                                                                   // Se actualiza la consola

    while(w){                                                                    //Ciclo continuo del menú
        
        int opcion,r;                                                            // Declaracion de las variables
    
        printw("------------Veterinaria Juano's------------\n\n");               // Impresion del menú
        printw("Opciones: \n");
        printw("    1. Ingresar Registro\n");
        printw("    2. Ver Registro\n");
        printw("    3. Eliminar Registro\n");
        printw("    4. Buscar Registro\n");
        printw("    5. Salir\n");
        printw("\n\n");
        printw("Digite su opción: ");

        scanw("%i", &opcion);                                                    // Lee la opcion ingresada

        switch (opcion)                                                          // Seleccion de la funcion se gun la opcion ingresada
        {
        case 1:
            r = ingresarRegistro();                                              // Ejecuta la función de Ingreso que se encuentra en SourceCode/uno.c

            if (r!=0) {printw("Registro fallido.\n");}
            else {printw("Registro exitoso.\n");} 
            break;

        case 2:
            r = verRegistro();                                                   // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;

        case 3:
            r = eliminarRegistro();                                              // Ejecuta la función de eliminarRegistro que se encuentra en SourceCode/tres.c
            break;

        case 4:
            r = buscarRegistro();                                                // Ejecuta la función de verRegistro que se encuentra en SourceCode/dos.c
            break;
        
        case 5:
            printw("Gracias.\n");                                                //Imprime gracias y termina el programa.
            w = 0;
            break;
        
        default:
            printw("Opcion invalida.\n");                                        //Imprime el mensaje.
        }

        keyToContinue();

    }
    clear(); 
    endwin();
    return 0;
}