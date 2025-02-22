/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
/*part3 et 4, simple command and redirections*/
void cmd_simple(struct cmdline* l,char **cmd) {
	pid_t pid = Fork();
	int in, out = 0;
	if (pid==0) { //execute from the child
		if (l->out){ 		// test if there is a redirection out
			out = Open(l->out, O_CREAT | O_WRONLY | O_TRUNC , 0644);
			fprintf(stderr, "Debug - hello there is a redirection out here\n");
			Dup2(out,1);	//on redirige la sortie standard vers le fichier
			Close(out);
		} 
		if (l->in){		// test if there is a redirection out
			in = Open(l->in, O_RDONLY, 0444);
			fprintf(stderr, "Debug - hello there is a redirection in here\n");
			Dup2(in,0);		//on redirige le fichier vers l'entrée standard
			Close(in);
		}
		if (execvp(cmd[0], &cmd[0])==-1){
			switch(errno) { //error managing
				case ENOENT: //errno: No such file or directory
					fprintf(stderr, "shell: command not found\n");
					break;
				case EINVAL:
					fprintf(stderr, "shell: invalid arguments\n");
					break;
				default:
					perror("shell ");
					break;
			}
			exit(0);
		}
	}
	else { //father
		waitpid(pid, NULL, 0);
	}
}

/* part 6*/
void pipe_cmd_simple(char **cmd1, char **cmd2) {
	int fd[2]; //fd[0] = read, fd[1] = write
	pid_t pid1, pid2;
	if (pipe(fd) == -1) {
		perror("Pipe error ");
		exit(EXIT_FAILURE);
	}
	if ((pid1 = Fork())==-1) {
		perror("Fork error ");
		exit(EXIT_FAILURE);
	}

	//first command
	if (pid1 == 0) {
		Close(fd[0]); //close the pipe read
		Dup2(fd[1], STDOUT_FILENO); //make stdout points to fd
		Close(fd[1]);
		execvp(cmd1[0], &cmd1[0]);
		perror("execvp ");
		exit(EXIT_FAILURE);

	}
	if ((pid2 = Fork())==-1) {
		perror("Fork error ");
		exit(EXIT_FAILURE);
	}	
	//second command
	if (pid2 == 0) {
		Close(fd[1]); //close the pipe write
		Dup2(fd[0], STDIN_FILENO); //makes stdin points to fd
		Close(fd[0]);
		execvp(cmd2[0], &cmd2[0]);
		perror("execvp ");
		exit(EXIT_FAILURE);
	}
	//father
	Close(fd[0]);
	Close(fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	return;
}

int main()
{

	while (1) {
		struct cmdline *l;
		int i;// j;//, k;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		/*Here we checking if the line isn't empty*/
		if (l->seq[0] == NULL){
			continue;
		}
		
		/* If the first command is 'quit' then we exit the shell program */
		if (strcmp(l->seq[0][0],"quit")==0){
			exit(0);
		}
		
		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Execute each command of the pipe */
		//for (i=0; l->seq[i]!=0; i++) {
			i = 0;
			char **cmd1 = l->seq[i];
			if (l->seq[i+1]!=0) {
				char **cmd2 = l->seq[i+1];
				pipe_cmd_simple(cmd1, cmd2);
			} else {
				cmd_simple(l, cmd1);
			}
			/*printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("command of the pipe: \n");
				printf("cmd[%d]: %s ", j, cmd[j]);
				printf("command of the sequence: \n");
				//for (k=0; cmd[j][k]!=0; k++) {
				//	printf("%s ", cmd[j][k]);
				//}
			}
			printf("\n");
			//cmd_simple(l,cmd);*/
		//}
	}
}
