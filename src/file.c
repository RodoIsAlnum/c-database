#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"


int create_db_file(char *filename) {
	//manejo de creación de la base de datos
	//flujo:
	//	- comprobar que el archivo existe
	//		- si sí, cerrarlo, declarar existencia y cerrar el programa
	//		- si no, crear el archivo como si nada
	
	int fileDescriptor = open(filename, O_RDWR, 0644); // para comprobar existencia
	if (fileDescriptor != -1) {
		close(fileDescriptor);
		printf("File already exists.\n");
		return STATUS_ERROR;
	}

	fileDescriptor = open(filename, O_RDWR | O_CREAT, 0644); // crear, leer y escribir con operador OR ( | )
	if (fileDescriptor == -1) {
		//printf("Couldn't create file\n");
		perror("open");
		return STATUS_ERROR;
	}

	return fileDescriptor;
}

int open_db_file(char *filename) {
	//manejo de creación de la base de datos
	//flujo:
	//	- comprobar que el archivo existe
	//		- si sí, abrirlo, como si nada
	//		- si no, simplemente declarar no existencia y cerrar
	
	int fileDescriptor = open(filename, O_RDWR, 0644); // crear, leer y escribir con operador OR ( | )
	if (fileDescriptor == -1) {
		//printf("Couldn't create file\n");
		perror("open");
		return STATUS_ERROR;
	}

	return fileDescriptor;
}


