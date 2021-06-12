#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define READ_END 0
#define WRITE_END 1


//se ejecutarán 3(forks) procesos, se comunicarán con pipes 


//dup2(fd_old,fd_nex)

//lo que ejecute el primer comando "ls" escriba en el file descriptor stdout, se escriba en la pipe(1)

//	ls | cd Documentos | pwd 

int main(){

	int fd1[2], fd2[2];
	int status, pid;
	
	pipe(fd1);
	pid=fork();
	
	if(pid==0){
		close(fd1[READ_END]); //SE CIERRA EXTREMO NO NECESARIO
		
		dup2(fd1[WRITE_END],STDOUT_FILENO);
		close(fd1[WRITE_END]);
		
		execlp("/bin/ls","ls",NULL);
	}else{
		close(fd1[WRITE_END]);
		
		pipe(fd2);
		pid=fork();
		
		if(pid==0){
			close(fd2[READ_END]);
			
			dup2(fd1[READ_END],STDIN_FILENO);
			close(fd1[READ_END]);
			
			dup2(fd2[WRITE_END],STDOUT_FILENO);
			close(fd2[WRITE_END]);
			
			execlp("/bin/cd","cd","Documentos",NULL);
		}else{
			close(fd1[READ_END]);
			close(fd2[WRITE_END]);
			
			pid=fork();
			
			if(pid==0){
				dup2(fd2[READ_END],STDIN_FILENO);
				close(fd2[READ_END]);
				
				execlp("/bin/pwd/","pwd","",NULL);
			}
		}
	}
	
	close(fd2[READ_END]);
	
	wait(&status);
	wait(&status);
	wait(&status);

	return 0;
}
