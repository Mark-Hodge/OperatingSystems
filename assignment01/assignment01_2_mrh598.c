#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <sys/time.h>
  
int main() 
{
	struct timeval start, end;
	long switch_micros;

	for (int i = 0; i < 25; i++)
	{
		// Get time at start of single context switch
		gettimeofday(&start, NULL);
	  
		// Create two pipes each with a seperate read/write end
		int fd1[2];
		int fd2[2];
	  
		char parent_string[] = "parent_string"; 
		char child_string[]  = "child_string"; 
		pid_t p; 
	  
		if (pipe(fd1)==-1) 
		{ 
			fprintf(stderr, "Pipe Failed" ); 
			return 1; 
		} 
		if (pipe(fd2)==-1) 
		{ 
			fprintf(stderr, "Pipe Failed" ); 
			return 1; 
		} 
	  
		// Fork processes
		p = fork(); 
	  
		if (p < 0) 
		{ 
			fprintf(stderr, "fork Failed" ); 
			return 1; 
		} 
	  

		// Parent process 
		else if (p > 0) 
		{			
			char parent_inbuf[100]; 
		  
			close(fd1[0]);  // Close reading end of first pipe 
		  
			// Write input string and close writing end of first pipe
			write(fd1[1], parent_string, strlen(parent_string)+1);
			printf("Writing %s from parent to child\n", parent_string);
			close(fd1[1]); 
		  
			// Wait for child to send a string 
			wait(NULL); 
		  
			close(fd2[1]); // Close writing end of second pipe 
		  
			// Read string from child, print it and close reading end
			read(fd2[0], parent_inbuf, 100); 
			printf("Reading %s from child to parent\n", parent_inbuf);
			close(fd2[0]);

		} 
	  
		// child process 
		else
		{ 

			close(fd1[1]);  // Close writing end of first pipe 
		  
			// Read a string using first pipe 
			char child_inbuf[100]; 
			read(fd1[0], child_inbuf, 100); 
		  	printf("Reading %s from parent to child\n", child_inbuf);
			close(fd1[0]);

			// Write child string to parent
			close(fd2[0]);

			write(fd2[1], child_string, strlen(child_string)+1);
			printf("Writing %s from child to parent\n", child_string);
			close(fd2[1]);
	
			exit(1);

		} 

		// Get time at end of single complete context switch
		gettimeofday(&end, NULL);

		// Compute the time taken to complete the context switch
		switch_micros = (end.tv_usec - start.tv_usec);
		printf("\n-------Time for complete context switch: %ld microseconds-------\n\n", switch_micros);

	}

} 
