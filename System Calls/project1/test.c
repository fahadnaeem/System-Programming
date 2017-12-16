//Ahmet Türk	150120107
//Tuğba Özkal	150120053
//--------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define setmyflag 355

int main () {
	int flag, i;
	pid_t child;
	printf("Test process id: %d\n", getpid());
	
	printf("Enter value of flag: ");
	scanf("%d", &flag);
	
	int err = syscall(setmyflag, getpid(), flag);
	if (err == EACCES) {
		printf("You need root privileges!\n");
	} 
	else if(err == ESRCH){
		printf("Flag value should be 0 or 1!\n");
	}
	else if(err == EINVAL){
		printf("There is no such process!\n");
	}
	else {
		printf("System call return: %d\n", err);
	}
	
	
	for (i = 0; i < 2; i++) { // create two children
		child = fork();
		if (child == 0) {
			break;
		}
	}
	
	if (child  == 0) {  // child
		printf("Child pid: %d, parent pid: %d\n", getpid(), getppid());
		while(1);
	}
	else {				// parent
		printf("Press enter to kill test process\n");
		getchar();
		getchar();
		printf("Test process is about to exit\n");
		exit(0);
	}

	return 0;
}
