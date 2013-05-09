#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"

int readJobs() {
	FILE* f;
	char* line = NULL;
	size_t n = 0;

	f = fopen("job", "r");

	if (f == NULL)
		perror("Error opening file\n");
	else {
		while(getline(&line, &n, f) != -1) {
			printf("%s\n", line);
		}
	
		if (ferror(f))
			perror("Error reading from file\n");
		}

	fclose(f);

	return 1;
}


int main(void) {

	readJobs();

	return 1;
}


