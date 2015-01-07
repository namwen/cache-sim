#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
char *readFile(char *filename) {
	
	struct stat st;
	int character;
	char *data;
	FILE *file;
	size_t index = 0;
	stat(filename, &st);	
    data = malloc(st.st_size);
    file = fopen(filename, "r");	
    if( file == NULL) {
		fprintf(stderr, "File \"%s\" could not be opened.\n", filename);
		return NULL;
	}

	while((character = fgetc(file)) != EOF) {
		data[index++] = (char) character;	
	}		

	data[index] = '\0';
	return data;
}
