/*
 * Filename	: MySystem.c
 * Author	: Christian Shepperson
 * Course	: CSCI 380
 * Assignment	: Redirection
 * Description	: Implement Linux function
 */

//Libraries
/*******************/
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


int mysystem (const char* command);
int main(int argc, char* argv[]){
     if(argc != 2){
        printf("Please only have one argument\n");
        exit(1);
    }
    int x;

    x = mysystem(argv[1]);
    
    return x;
}


int mysystem(const char* command){

    __pid_t pid = fork();

    

   if (pid == 0)
	{
		int red = open("System.out", O_WRONLY | O_CREAT | O_TRUNC, 0600);
		if (red < 0)
		{
			printf("Unable to create file System.out");
			exit(1);
		}
		dup2(red, 1); // Redirect stdout to opened file
		close(red);

		int exe = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		if (exe < 0)
		{
			fprintf(stderr, "exec error (%s) -- exiting\n", strerror (errno));
			exit(1);
		}
	}
	int status;
	pid_t child = waitpid(pid, &status, 0);
	if(child == pid){
	if(WIFEXITED(status)){

			printf("Exit status: %d\n", WEXITSTATUS(status));
	}
		else if( WIFSIGNALED(status)){

			printf("Exit status: %d\n", WTERMSIG(status));;
		}
	}	
	return status;
}

