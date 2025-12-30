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

int list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	for (int i = 0; i < dbhdr->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n",employees[i].name);
		printf("\tAddress: %s\n",employees[i].address);
		printf("\tHours: %d\n",employees[i].hours);
	}
	return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
	if (NULL == dbhdr) return STATUS_ERROR;
	if (NULL == employees) return STATUS_ERROR;
	if (NULL == *employees) return STATUS_ERROR;
	if (NULL == addstring) return STATUS_ERROR;

	char *name = strtok(addstring, ",");  // strtok divide cadenas en subcadenas utilizando un carácter separador.
	if (NULL == name) return STATUS_ERROR;

	char *addr =  strtok(NULL, ",");
	if (NULL == addr) return STATUS_ERROR;

	char *hours = strtok(NULL, ",");
	if (NULL == hours) return STATUS_ERROR;


		
	struct employee_t *e = *employees;
	e = realloc(e,sizeof(struct employee_t)*dbhdr->count+1);
	if (e == NULL) {
		return STATUS_ERROR;
	}

	dbhdr->count++;

	strncpy(e[dbhdr->count-1].name,name,sizeof(e[dbhdr->count-1].name));
	strncpy(e[dbhdr->count-1].address,addr,sizeof(e[dbhdr->count-1].address));
	e[dbhdr->count-1].hours = atoi(hours);


	
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("File descriptor is not valid\n");
		return STATUS_ERROR;
	}

	int count = dbhdr->count;

	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == NULL) {
		printf("malloc has failed\n");
		return STATUS_ERROR;
	}

	read(fd, employees, count*sizeof(struct employee_t));

	for (int i = 0; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		printf("The file descriptor is not valid\n");
		return STATUS_ERROR;
	}

	if (dbhdr == NULL) {
		printf("header seems to be invalid\n");
		return STATUS_ERROR;
	}

	int realcnt = dbhdr->count;

	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcnt));
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	lseek(fd, 0, SEEK_SET);
	write(fd, dbhdr, sizeof(struct dbheader_t));


	for (int i = 0; i < realcnt; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

	return STATUS_SUCCESS;
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


