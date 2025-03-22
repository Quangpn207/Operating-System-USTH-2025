#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "readcmd.h"


void trace_cmd(char ***cmd) {
		int i, j;
		/* Display each command of the pipe */
		/* cmd is a table of table of strings, each table ends with a NULL element */
		printf("trace_cmd\n");
		for (i=0; cmd[i]!=0; i++) {
			char **cmdi = cmd[i];
			printf("cmd[%d]: ", i);
			for (j=0; cmdi[j]!=0; j++) {
				printf("(%s) ", cmdi[j]);
			}
			printf("\n");
		}
}


int main()
{
	int spid, status;
	char ***cmd;
	
	while(1) {
		printf("shell> ");

		cmd = readcommand();
		trace_cmd(cmd);
		
		if (! cmd[0]) continue;


		if(!strcasecmp(cmd[0][0], "exit")) {
			return 0;
		}
		if(cmd[1]==NULL){
			pid_t pid = fork();
			if(pid == -1){
				perror("Fork failed");
				continue;
			}
			if (pid == 0){
				execvp(cmd[0][0], cmd[0]);
				perror("Exec failed");
				exit(1);
			}
			else{
				waitpid(pid, NULL, 0);
			}
		}
		else if(cmd[1]!=NULL && cmd[2] == NULL){
			int fd[2];
			if(pipe(fd) == -1){
				perror("Pipe failed");
				continue;
			}
			pid_t pid1 = fork();
			if(pid1 == -1){
				perror("Fork failed");
				continue;
			}
			if(pid1 == 0){
				close(fd[0]);
                dup2(fd[1], 1);
                close(fd[1]);
                execvp(cmd[0][0], cmd[0]);
                perror("Exec failed");
                exit(1);
			}
			pid_t pid2 = fork();
			if(pid2 == -1){
				perror("Fork failed");
				continue;
			}
			if(pid2 == 0){
				close(fd[1]);
				dup2(fd[0], 0);
				close(fd[0]);
				execvp(cmd[1][0], cmd[1]);
				perror("Exec failed");
				exit(1);
			}
			close(fd[0]);
			close(fd[1]);
			waitpid(pid1, NULL, 0);
			waitpid(pid2, NULL, 0);
		}
		else{
			printf("Pipeline > 2 commands not implements\n");
			}
	}
	return 0;
}