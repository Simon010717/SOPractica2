#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SIZE 32
#define NAMES 1700


void readFile(FILE* file, char* AllNames[]){
    for(int j = 0; j < NAMES; j++){
        AllNames[j] = malloc(SIZE);
        int i = 0;
        char *a;
        do{
            a = AllNames[j]+i;
            fread(a,1,1,file);
            i++;
        }while(*a != '\n'  && i<SIZE);
        *a = '\0';
    }
        
}

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

int hashf(char word[],int l){
    int m,h,i,x,p;
    m = 1000;

    p = 1009;
    x = 22;

    for(i=0;i<l;i++){
        word[i] = tolower(word[i]);
    }

    h = 1;
    for(i=0; i<l; i++){
		h = (p + h*x+(int)word[i])%p;
    }

    return h%m;
}

int main(){
    FILE *names;
	char* AllNames[NAMES];

	names = fopen("nombres.txt", "r");
    if(names == NULL){
        perror("No se encontro el archivo de nombres.\n");
        exit(EXIT_FAILURE);
    }

    readFile(names,AllNames);
    fclose(names);

	char* Razas[] = {"Husky", "Labrador", "Pomerania", "Buldog", "Golden Retriever", "Pastor Aleman", "Pug", "Doberman", "Beagle"};
    char Generos[] = {'H', 'M'};
    int Edades[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    float Pesos[] = {5.5f, 10.2f, 15.7f, 20.0f, 25.7f, 30.3f};
    int Estaturas[] = {3, 4, 5, 6, 7, 8, 9, 10};

	for ( int i = 0; i < 10000000; i++ ){
		int r,pos,id,hash,size;                                                   
		int *tempid;                                                   

		struct dogType *data;                                                   
		data = malloc(sizeof(struct dogType));                                                   


		bzero(data->nombre,32);
		strcpy(data->nombre, AllNames[i%NAMES]);
		//printf("%s",data->nombre);

		sprintf(data->tipo, "Perro");

		data->edad = Edades[i%15];

		sprintf(data->raza, "%s", Razas[i%9]);

		data->estatura = Estaturas[i%15];

		data->peso = Pesos[i%6];

		data->sexo = Generos[i%2];                                      

		hash = hashf(data->nombre,32);                                                   

		char* dir;                                      
		dir = malloc(15);                                       
		bzero(dir,15);                                      
		char num[3];                                      
		strcat(dir,"../hash/");                                      
		sprintf(num,"%i",hash);                                      
		strcat(dir,num);                                      
		
		//printf("%s",dir);

		FILE *fp;                                      
		fp = fopen(dir,"a");                                      
		size = ftell(fp);                                      
		fclose(fp);                                      

		id = 1000*(int)(size/36)+hash;                                      
		data->id = id;                                      

		fp = fopen(dir,"a");                                      
		fwrite(&id,sizeof(int),1,fp);                                      
		fwrite(data->nombre,32,1,fp);                                      

		fclose(fp);                                      

		fp = fopen("../dataDogs.dat","a");                                      

		r = fwrite(data,sizeof(struct dogType),1,fp);                                      

		fclose(fp);                                      

		free(data);                                      
		free(dir);                                                                           

	}

	return 0;
}