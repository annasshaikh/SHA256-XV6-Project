#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
	int p1[2]; // Made 2 File Discripter
	int p2[2];

	pipe(p1); // Made Pipes
	pipe(p2);

	char BYTE = '#';
	
    int pid = fork();

	if(pid > 0) {  // Parent 
		close(p1[0]);
		write(p1[1], &BYTE, 1);
		
        char getfromChild;

		read(p2[0], &getfromChild, 1);

		printf("%d: received %d\n", getpid(),getfromChild);
		
        exit(0);
	} else if (pid == 0) { // Child
	    wait((int*)0); //Wait for parent To Complete
		char receive_byte;
		close(p1[1]);
		read(p1[0], &receive_byte, 1);

		printf("%d: received %d\n", getpid(), receive_byte);
		
        close(p2[0]);
		write(p2[1], &receive_byte, 1);
		close(p2[1]);
		exit(0);
	}else{
        printf("ERROR\n");
    }
}