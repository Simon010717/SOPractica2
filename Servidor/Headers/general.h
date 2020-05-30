#ifndef GENERALH
#define GENERALH
#include <stdio.h>
#include <stdbool.h>

struct dogType{
   int id;
   char nombre[32];
   char tipo[32];
   int edad;
   char raza[16];
   int estatura;
   float peso;
   char sexo;
};

int hashf(char word[],int l);

#endif