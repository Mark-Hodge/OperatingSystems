#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main() {

	struct timeval timeValue;

	for (int i = 0; i < 50; i++) {
		gettimeofday(&timeValue, NULL);

		printf("%ld\t%ld\n", timeValue.tv_usec, timeValue.tv_sec);
	}
	
	return 0;
}
