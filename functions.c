#include "structs.h"
#include "functions.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

int isInt(char* numero){
  /*VERIFICACION DE QUE EL ARGUMENTO numero SEA ENTERO*/
 for(int i=0;i<strlen(numero);i++){
   if(isdigit(numero[i])==0){
     return 0;
   }
 }
 return 1;
}

char* setNameInput(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"imagen_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}

char* setNameOutput(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"output_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}

char* setNameOutputGS(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"outputGS_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}

char* setNameOutputBin(int a){
  char* nombreEntrada=malloc(sizeof(char)*20);
  strcpy(nombreEntrada,"outputBin_");

  char* buffer=malloc(sizeof(char)*12);
  sprintf(buffer,"%d",a);

  char* formato=".bmp";

  char* nombreDef=malloc(sizeof(char)*22);
  nombreDef=strcat(nombreEntrada,buffer);
  //printf("nombre parcial: %s\n",nombreDef);
  nombreDef=strcat(nombreDef,formato);
  //printf("%s\n",nombreDef);
  return nombreDef;
}

unsigned char* loadBMP(char* filename, bmpInfoHeader* bInfoHeader, bmpFileHeader* header){
  FILE* f;
 						     /* cabecera */
  unsigned char* imgdata;   /* datos de imagen */
  uint16_t type;        /* 2 bytes identificativos */

  f=fopen (filename, "r");
  if (!f)
    return NULL;        /* Si no podemos leer, no hay imagen*/

  /* Leemos los dos primeros bytes */
  fread(&type, sizeof(uint16_t), 1, f);
  if (type !=0x4D42)        /* Comprobamos el formato */
    {
      fclose(f);
      return NULL;
    }

  /* Leemos la cabecera de fichero completa */
  fread(header, sizeof(bmpFileHeader), 1, f);

  /* Leemos la cabecera de información completa */
  fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

  /* Reservamos memoria para la imagen, ¿cuánta?
     Tanto como indique imgsize */
  imgdata=(unsigned char*)malloc(bInfoHeader->imgsize);

  /* Nos situamos en el sitio donde empiezan los datos de imagen,
   nos lo indica el offset de la cabecera de fichero*/
  fseek(f, header->offset, SEEK_SET);

  /* Leemos los datos de imagen, tantos bytes como imgsize */
  fread(imgdata, bInfoHeader->imgsize,1, f);


  /* Cerramos */
  fclose(f);

  /* Devolvemos la imagen */
  return imgdata;
}

//Funcion que guarda una matriz de pixeles en formato bmp
void saveImage(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename){
	//Abrimos el archivo de la nueva imagen
  int imagen= open(filename, O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  //printf("%d\n",imagen);
  //FILE* imagen = fopen(filename, "w");
	if(imagen<0){
		printf("Error: %d.\n",errno);
		return;
	}
	//Escribimos el tipo de archivo (BM)
	uint16_t type = 0x4D42;
  //fwrite(&type, sizeof(uint16_t), 1, imagen);
  if(write(imagen, &type, sizeof(uint16_t)) != sizeof(uint16_t) ){
      write(2,"There was an error writing to standard out\n", 44);
  }

  //Escribimos la cabecera del archivo completa
	//fwrite(&header, sizeof(bmpFileHeader),1,imagen);
  write(imagen, &header, sizeof(bmpFileHeader) );

  //Escribimos la cabecera de info de la imagen completa
	//fwrite(&bInfoHeader, sizeof(bmpInfoHeader),1,imagen);
  write(imagen, &bInfoHeader, sizeof(bmpInfoHeader) );

  //Nos movemos a la parte del archivo en donde deben ir los datos, segun el header del archivo
	//fseek(imagen, header.offset, SEEK_SET);
  //lseek(int fd, off_t offset, int whence);
  if(lseek(imagen, header.offset, SEEK_SET)<0){
    printf("se cae en lseek\n");
  }

  //escribimos los datos de la imagen
	//fwrite(array, bInfoHeader.imgsize,1, imagen);
  write(imagen, array, bInfoHeader.imgsize );
  //cerramos el archivo de salida
	if(close(imagen)<0){
    printf("falla en close()\n");
  }
}

unsigned char* rgbToGrayScale(unsigned char* array, bmpInfoHeader bInfoHeader){
  int i,j,y,azul,verde,rojo,indice=0;
  //SE TRATA LA IMAGEN CON LA FORMULA
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
			     //los componentes alfa, r*0.3, g*0.59 ,b*0.11 de un pixel
           azul=array[indice]*0.11;//azul
           indice++;
           verde=array[indice]*0.59;//verde
           indice++;
           rojo=array[indice]*0.3;//rojo
           y=(azul+verde+rojo);
           indice++;//estoy en alpha
           array[indice-3]=y;
           array[indice-2]=y;
           array[indice-1]=y;
           indice++;//estoy en blue del proximo j
		  }
	}
  return array;
}

//Funcion que guarda una matriz de pixeles en formato bmp
void saveImageGS(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename){
  /*
  FILE* imagen = fopen(filename, "w");
	if(imagen==NULL){
		printf("Error de memoria en la creacion del archivo de imagen de salida.\n");
		return;
	}
	//Escribimos el tipo de archivo (BM)
	uint16_t type = 0x4D42;
	fwrite(&type, sizeof(uint16_t), 1, imagen);
	//Escribimos la cabecera del archivo completa
	fwrite(&header, sizeof(bmpFileHeader),1,imagen);
	//Escribimos la cabecera de info de la imagen completa
	fwrite(&bInfoHeader, sizeof(bmpInfoHeader),1,imagen);
	//obtenemos el array de datos de la imagen
	//unsigned char* datos_imagen = transformarMatriz(pixeles);
	//Escribimos el arreglo de datos de la imagen en el archivo
	//Nos movemos a la parte dle archivo en donde deben ir los datos, segun el header del archivo
	fseek(imagen, header.offset, SEEK_SET);
  rgbToGrayScale(array,bInfoHeader);
  //escribimos los datos de la imagen
	fwrite(array, bInfoHeader.imgsize,1, imagen);
	//cerramos el archivo de salida
	fclose(imagen);
  */
  //Abrimos el archivo de la nueva imagen
  int imagen= open(filename, O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  //printf("%d\n",imagen);
  //FILE* imagen = fopen(filename, "w");
	if(imagen<0){
		printf("Error: %d.\n",errno);
		return;
	}
	//Escribimos el tipo de archivo (BM)
	uint16_t type = 0x4D42;
  //fwrite(&type, sizeof(uint16_t), 1, imagen);
  if(write(imagen, &type, sizeof(uint16_t)) != sizeof(uint16_t) ){
      write(2,"There was an error writing to standard out\n", 44);
  }

  //Escribimos la cabecera del archivo completa
	//fwrite(&header, sizeof(bmpFileHeader),1,imagen);
  write(imagen, &header, sizeof(bmpFileHeader) );

  //Escribimos la cabecera de info de la imagen completa
	//fwrite(&bInfoHeader, sizeof(bmpInfoHeader),1,imagen);
  write(imagen, &bInfoHeader, sizeof(bmpInfoHeader) );

  //Nos movemos a la parte del archivo en donde deben ir los datos, segun el header del archivo
	//fseek(imagen, header.offset, SEEK_SET);
  //lseek(int fd, off_t offset, int whence);
  if(lseek(imagen, header.offset, SEEK_SET)<0){
    printf("se cae en lseek\n");
  }
  rgbToGrayScale(array,bInfoHeader);
  //escribimos los datos de la imagen
	//fwrite(array, bInfoHeader.imgsize,1, imagen);
  write(imagen, array, bInfoHeader.imgsize );
  //cerramos el archivo de salida
	if(close(imagen)<0){
    printf("falla en close()\n");
  }

}

unsigned char* binarizeImage(unsigned char* array, bmpInfoHeader bInfoHeader,int umbral){
  int i,j,prom,azul,verde,rojo,indice=0;
  //SE TRATA LA IMAGEN CON LA FORMULA
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
           azul=array[indice];//azul
           indice++;
           verde=array[indice];//verde
           indice++;
           rojo=array[indice];//rojo
           prom=(azul+verde+rojo)/3;
           indice++;//estoy en alpha
           if(prom>umbral){
             array[indice-3]=255;
             array[indice-2]=255;
             array[indice-1]=255;
           }
           else{
             array[indice-3]=0;
             array[indice-2]=0;
             array[indice-1]=0;
           }
           indice++;//estoy en blue del proximo j
		  }
	}
  return array;
}

//Funcion que guarda una matriz de pixeles en formato bmp
void saveImageBin(unsigned char* array, bmpInfoHeader bInfoHeader, bmpFileHeader header, char* filename,int umbral){
  /*
  FILE* imagen = fopen(filename, "w");
	if(imagen==NULL){
		printf("Error de memoria en la creacion del archivo de imagen de salida.\n");
		return;
	}
	//Escribimos el tipo de archivo (BM)
	uint16_t type = 0x4D42;
	fwrite(&type, sizeof(uint16_t), 1, imagen);
	//Escribimos la cabecera del archivo completa
	fwrite(&header, sizeof(bmpFileHeader),1,imagen);
	//Escribimos la cabecera de info de la imagen completa
	fwrite(&bInfoHeader, sizeof(bmpInfoHeader),1,imagen);
	//obtenemos el array de datos de la imagen
	//unsigned char* datos_imagen = transformarMatriz(pixeles);
	//Escribimos el arreglo de datos de la imagen en el archivo
	//Nos movemos a la parte dle archivo en donde deben ir los datos, segun el header del archivo
	fseek(imagen, header.offset, SEEK_SET);
  binarizeImage(array,bInfoHeader,umbral);
  //escribimos los datos de la imagen
	fwrite(array, bInfoHeader.imgsize,1, imagen);
	//cerramos el archivo de salida
	fclose(imagen);*/
  //Abrimos el archivo de la nueva imagen
  int imagen= open(filename, O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  //printf("%d\n",imagen);
  //FILE* imagen = fopen(filename, "w");
	if(imagen<0){
		printf("Error: %d.\n",errno);
		return;
	}
	//Escribimos el tipo de archivo (BM)
	uint16_t type = 0x4D42;
  //fwrite(&type, sizeof(uint16_t), 1, imagen);
  if(write(imagen, &type, sizeof(uint16_t)) != sizeof(uint16_t) ){
      write(2,"There was an error writing to standard out\n", 44);
  }

  //Escribimos la cabecera del archivo completa
	//fwrite(&header, sizeof(bmpFileHeader),1,imagen);
  write(imagen, &header, sizeof(bmpFileHeader) );

  //Escribimos la cabecera de info de la imagen completa
	//fwrite(&bInfoHeader, sizeof(bmpInfoHeader),1,imagen);
  write(imagen, &bInfoHeader, sizeof(bmpInfoHeader) );

  //Nos movemos a la parte del archivo en donde deben ir los datos, segun el header del archivo
	//fseek(imagen, header.offset, SEEK_SET);
  //lseek(int fd, off_t offset, int whence);
  if(lseek(imagen, header.offset, SEEK_SET)<0){
    printf("se cae en lseek\n");
  }
  binarizeImage(array,bInfoHeader,umbral);
  //escribimos los datos de la imagen
	//fwrite(array, bInfoHeader.imgsize,1, imagen);
  write(imagen, array, bInfoHeader.imgsize );
  //cerramos el archivo de salida
	if(close(imagen)<0){
    printf("falla en close()\n");
  }
}

char* nearlyBlack(unsigned char* array, bmpInfoHeader bInfoHeader,int umbralPorcentaje){
  int i,j,prom,azul,verde,rojo,negro,blanco,indice=0;
  negro=0;
  blanco=0;
  //Se recorre segun ancho y largo
  for(i=0; i < bInfoHeader.height; i++){
			for(j=0; j < bInfoHeader.width; j++){
           azul=array[indice];//azul
           indice++;
           verde=array[indice];//verde
           indice++;
           rojo=array[indice];//rojo
           prom=(float)(azul+verde+rojo)/3;
           indice++;//estoy en alpha
           if(prom==0){
             negro++;
           }
           else{
             blanco++;
           }
           indice++;//estoy en blue del proximo j
		  }
	}
  if((float)(negro/(negro+blanco))>=(float)(umbralPorcentaje/100)){
      return "yes";
  }
  else{
    return "no";
  }
}
