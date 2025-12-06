#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr /*, struct employee_t *employees*/) {
	if (fd < 0) {
		printf("The file descriptor is not valid\n");
		return STATUS_ERROR;
	}

	if (dbhdr == NULL) {
		printf("header seems to be invalid\n");
		return STATUS_ERROR;
	}



	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(dbhdr->filesize);
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	lseek(fd, 0, SEEK_SET);
	write(fd, dbhdr, sizeof(struct dbheader_t));

	//return;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	// validación del header
	// recibimos archivo y puntero de salida nuevamente
	// revisar el file descriptor para revisar su validez
	if (fd < 0) {
		printf("The file descriptor is not valid\n");
		return -1;
	}

	struct dbheader_t *header =  calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc has failed to allocate header\n");
		return -1;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return -1;
	}

	// desempaquetar header
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->version != 1) {
		printf("Invalid header version\n");
		free(header);
		return -1;
	}
	if (header->magic != HEADER_MAGIC) {
		printf("Invalid magic number\n");
		free(header);
		return -1;
	}	

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header-> filesize != dbstat.st_size) {
		printf("Invalid file size, database may be corrupt\n");
		free(header);
		return -1;
	}

	*headerOut = header;

	return 0;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
	// creación del header de la base de datos
	// necesitamos el archivo, y un puntero del header
	// inicializamos un stuct para el header
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // pointer para guardar en el headerOut
	if (header == NULL) {
		printf("Malloc has failed to allocate\n");
		return -1;
	}
	
	
	// síntaxis de estructuras de los punteros
	// structVar -> miembroDelStruct = valor;
	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header; // devolvemos el header al archivo y retoramos éxito

	return 0;

}


