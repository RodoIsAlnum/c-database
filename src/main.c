#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	// imprimir el uso UNIX del programa
	printf("USAGE: %s -n -f <db file>\n", argv[0]);
	printf("\t-n - Create a new database file\n");
	printf("\t-f - Opens an existent database file\n");
	printf("\t-a <Emp data> - Adds a new employee to the database\n");

}


int main(int argc, char *argv[]) { 
	int flagOpt;
	char *filepath = NULL;
	char *newBuffer = NULL;
	bool isNewFile = false;
	bool list = false;
	struct employee_t *employees;

	int dbfd = -1;
	struct dbheader_t *header = NULL;
	
	// conseguir los valores de getopt
	while ((flagOpt = getopt(argc, argv,"nf:a:l")) != -1) {
		switch(flagOpt) {
			case 'f':
				filepath = optarg;	// optarg proviene del propio uso de la función
						  	// espera que seguido de -f haya un argumento adicional.
				break;
			case 'n':
				isNewFile = true;
				break;
			case 'a':
				newBuffer = optarg;
				break;
			case '?':			// cualquier valor que no sea esperado
				printf("Unknown option -%c\n", flagOpt);
				break;
			case 'l':
				list = true;
				break;
			default:
				return -1;		// simplemente dar error, no debería caer en default
		}
	}

	if (filepath == NULL) {
		printf("No file provided\n");
		print_usage(argv);
		return 0;
	}

	
	// testear por newfile
	if (isNewFile) {
		dbfd = create_db_file(filepath);
		if (dbfd == -1) {
			printf("Couldn't create database file \n");
			return -1;
		}
		if (create_db_header(dbfd,&header) == -1) {
			printf("couldn't create database header\n");
		}
	} else {
		dbfd = open_db_file(filepath);
		if (dbfd == -1) {
			printf("Couldn't open database file \n");
			return -1;
		}
		//validar el encabezado del archivo
		if (validate_db_header(dbfd, &header) == -1) {
			printf("Validation for database header failed\n");
			return -1;
		}
	}

	if (read_employees(dbfd, header, &employees) != STATUS_SUCCESS) {
		printf("Failed to read employees");
		return -1;
	}

	if (newBuffer) {
		add_employee(header, &employees, newBuffer);
	}

	if (list) {
		list_employees(dbhdr,employees);
	}

	output_file(dbfd, header, employees);


	// debugging
	//printf("New file? %d\n", isNewFile);
	//printf("filepath: %s\n", filepath);

	
	return 0;
	
}
