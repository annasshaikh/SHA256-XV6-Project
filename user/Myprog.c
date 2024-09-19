#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void){
	int before_system_calls = getcallcount();
	int pid = fork();
	if(pid > 0){
		wait((int*)0);
		printf("child %d is done\n", pid);
		printf("Total System Calls In this Program: %d\n" , getcallcount() - before_system_calls);
	} else if(pid == 0){
		printf("child: exiting\n");
		exit(0);
	}
}
