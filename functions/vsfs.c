#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#include "vsfs.h"


#define MAX_LENGTH 256		// Max line length
#define MAXLEN	80				// Max chars for param identifiers
#define COMMENT '#'				// Comment line in param file

/** @brief Get read of training and leading whitespaces
 *  including the \n from fgets().
 *  returns the clean line.
 *  Warning: line is modified.
 */
char *trim (char *line) {
	// Initialize pointers
	char *pStart = line;
	char *pEnd = &line [strlen (line) - 1];
	
	// Trim and delimit right side
	while ( isspace(*pEnd) && (pEnd >= pStart) ) pEnd--;
	*(pEnd + 1) = '\0';

	// Trim left side
	while ( isspace(*pStart) && (pEnd > pStart) ) pStart++;
	
	// Copy cleaned string
	strcpy (line, pStart);
	return line;
}

void initParams () {
	parameters.blockSize = BLOCKSIZE;
	parameters.iNodeBMSize = INODEBMSIZE;
	parameters.dataBMSize = DATABMSIZE;
	parameters.iNodeTabSize = INODETABSIZE;
	parameters.directCnt = DIRECTCNT;
	paramDebug = FALSE;
}


/** @brief doont re the file indicated in parameters and 
 *  populate the structure parameters
 */
void getParams (char* f_name){
	char *line, buffer [MAX_LENGTH];

	// Open file 
	FILE *fp = fopen (f_name, "r");
	if (fp == NULL) {
		printf ("Error opening file %s\n", f_name);
		return;	
	}
	// Read next line
	while ( (line = fgets (buffer, sizeof (buffer), fp)) != NULL) {
		// Skip comments and blank lines
		if (buffer [0] == '\n' || buffer [0] == COMMENT) continue;
		
		// Parse name/value pair from buffer
		char name [MAXLEN], value [MAXLEN];
		int intValue;
		
		line = strtok (buffer, "=");
		if (line == NULL) continue;
		
		strncpy (name, line, MAXLEN);
		line = strtok (NULL, "=");
		if (line == NULL) continue;
		strncpy (value, line, MAXLEN);
		
		trim (name);
		trim (value);
		intValue = atoi(value);
		
	// Reserve space for the currDir. Will b
    }
}