/*******************************
 * Author: Michael Adams
 * Last Edit: 4/22/18
 * Pitt CS 449
 ******************************/

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	// declare relevant vars
	int start;
	int end;
	int length;
	FILE *pi_dev;
	
	// if user entered 2 command line args, get digits of pi
	if ((argc <= 2) || (argc > 3)) {
		printf("ERROR: enter 2 arguments: [start] and [end] values\n");
		return(1);
	}

	// set start and end values from user input
	start = atoi(argv[1]);
	end = atoi(argv[2]);
	
	// check user entered start and end values
	if (start < 0) {
		printf("ERROR: <start value must be greater than or equal to 0\n");
		return 1;
	}
	else if (end < 0) {
		printf("ERROR: end value must be greater than or equal to 0\n");
		return 1;
	}
	else if (start > end) {
		printf("ERROR: end value must be greater than start value\n");
		return 1;
	}

	// calculate length and create a char array
	length = (end - start) + 1;
	char digits[length + 1];
	
	// open /dev/pi char device
	pi_dev = fopen("/dev/pi", "r");
	
	// seek to beginning of sequence and read data using pi_read
	fseek(pi_dev, start, SEEK_SET);
	fread(digits, 1, length, pi_dev);
	
	// close, append null term, print
	fclose(pi_dev);
	digits[length] = '\0';
	printf("%s\n", digits);
	
	return 0;
}