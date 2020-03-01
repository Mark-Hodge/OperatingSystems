#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sort.h"

void
usage(char *prog) {
	fprintf(stderr, "Usage: fastsort -i inputfile -o outputfile\n");
	exit(1);
}

struct Entries {
	unsigned int key;
	unsigned int records[24];
};

// Set up rules for comparison used by qsort()
int comparator(const void* p, const void* q) {
	
	
	const unsigned int pi = ((struct Entries*) p)->key;
	const unsigned int qi = ((struct Entries*) q)->key;

	if (pi < qi)
		return -1;

	else if(pi > qi)
		return 1;

	else
		return 0;
}

void openAndSort(char *inFile, char *outFile) {	
	// Open inFile for reading & outFile for writing.
	int inFD = open(inFile, O_RDONLY);
	if (inFD < 0) {
		fprintf(stderr, "Error: Cannot open file %s\n", inFile);
		exit(1);
	}

	int outFD = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	if (outFD < 0) {
		fprintf(stderr, "Error: Cannot open file %s\n", outFile);
		exit(1);
	}
	
	// Get size of inputfile in bytes.
	struct stat sb;
	fstat(inFD, &sb);
	int inFileBytes = sb.st_size;
	int numRecords = (inFileBytes / 100);

	// Go through key/records and put into sortable array
	struct Entries arr[numRecords];
	rec_t r;
	int counter = 0;
    while (1) {	
		int rc;
		rc = read(inFD, &r, sizeof(rec_t));
		if (rc == 0) // 0 indicates EOF
	    	break;
		if (rc < 0) {
	    	perror("read");
	    	exit(1);
		}

		arr[counter].key = r.key;					// Add key to entry

		int j;
		for (j = 0; j < NUMRECS; j++) {
			arr[counter].records[j] = r.record[j];	// Add element to record
		}
		counter++;
	}	

	// Invoke QSort on Entries structure
	printf("\nSorting entries by key...");
	qsort(arr, numRecords, sizeof(struct Entries), comparator);

	// Write the sorted records to outputfile.
	printf("\nWriting results to output file...");
	
	/* Convert structure backinto binary record format
	with key/records and write to output file. */
	rec_t r2;
	for (int i = 0; i < numRecords; i++) {
		r2.key = arr[i].key;

		for (int j = 0; j < NUMRECS; j++) {
			r2.record[j] = arr[i].records[j];
		}

		int rc = write(outFD, &r2, sizeof(rec_t));
		if (rc != sizeof(rec_t)) {
		perror("write");
		exit(1);
		}
	}printf("\nDone writing results. Program finished.\n");

	// Free space, close files, then exit.
	close(inFD);
	close(outFD);
	exit(0);
}

int main(int argc, char *argv[])
{
	// If too few/many arguments passed, call usage and exit.
	switch (argc) {
		case (5):
			//printf("\n -> HIT CASE 5\n"); // Delete later
			break;
		default:
			usage(argv[0]);
	}

	// Check for correct flags passed ('-i' and '-o')
	int flagsOk = 0;
	if (strcmp(argv[1], "-i") == 0) {
		if (strcmp(argv[3], "-o") == 0) {
			flagsOk = 1;
		}
	}

	// if flagsOK is true, proceed. Otherwise call usage and exit.
	switch (flagsOk) {
		case (1):
			//printf("\n -> inFlag IS '-i' and outFlag IS '-o'\n"); // Delete later
			openAndSort(argv[2], argv[4]);
			break;
		default:
			//printf("\n -> ERROR, inFlag IS NOT '-i' and/or outFlag IS NOT '-o'\n"); // Delete later
			usage(argv[0]);
	}
	return 0;
}
