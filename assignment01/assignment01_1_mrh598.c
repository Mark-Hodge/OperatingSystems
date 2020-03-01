#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main() {

	struct timeval start, end;
	ssize_t bytes_read;
	char data[0];
	double totalMicros = 0;
	double avgMicros = 0;


	// Execute 100 system calls, timing each call and total of all calls
	for (int i = 0; i < 100; i++) {
		gettimeofday(&start, NULL);

		// Execute system call (zero byte read)
		bytes_read = read(0, data, 0);

		gettimeofday(&end, NULL);


		long micros = (end.tv_usec - start.tv_usec);
		printf("%d Time elapsed is %ld microseconds\n", i, micros);
		totalMicros += micros;
	}

	// Calculate average time of all system calls
	avgMicros = (totalMicros / 100.0);
	printf("Average time of sys call after 100 read() operations is %f microseconds\n", avgMicros);

	return 0;
}
