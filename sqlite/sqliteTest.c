#include <stdio.h>
#include "sqlite3.h"

int callback(void* user_data, int argc, char** argv, char** field) {
	int i;
	int* counter = (int *)user_data;
	
	for(i=0; i<argc; i++){
		printf("%s = %s\n", field[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	(*counter)++;
	return 0;
}

