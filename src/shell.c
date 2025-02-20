/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

/*returns the position of the redirection, -1 if no*/
int contains_redir(char ** cmd) {
	for (int i=0; cmd[i] != NULL; i++) {
		if (strcmp(cmd[i],">")!=-1) return i;
	}
	return -1;
}

/*part3 et 4, simple command*/
void cmd_simple(struct cmdline* l,char **cmd) {
	/*factoriser plus tard dans une fonction a part entière*/
	pid_t pid = fork();
	int fd, fd1 = 0;
	if (pid==0) { //execute from the child
		if (l->out){ 		// test if there is a redirection out
			fd = Open(l->out, O_CREAT | O_WRONLY | O_TRUNC , 0644);
			printf("hello there is a redirection out here\n");
			printf("---------------------------------------------------------------------------------------------------------------------------\n");
			Dup2(fd,1);		//on redirige la sortie standard vers le fichier

			
		} 
		if (l->in){		// test if there is a redirection out
				fd1 = Open(l->in, O_RDONLY , 0644);
				printf("hello there is a redirection in here\n");
				printf("---------------------------------------------------------------------------------------------------------------------------\n");
				Dup2(fd1,0);	//on redirige le fichier vers l'entrée standard
		}

		if (execvp(cmd[0], &cmd[0])==-1){
			perror("execvp failed\n");
			close(fd);
			close(fd1);
			exit(0);
		}

		else {
			printf("hello from the child\n");
			printf("---------------------------------------------------------------------------------------------------------------------------\n");
			if (execvp(cmd[0], &cmd[0])==-1){
				perror("execvp failed\n");
				exit(0);
			}

		}	
	}
	waitpid(pid, NULL, 0);
	printf("---------------------------------------------------------------------------------------------------------------------------\n");
}



int main()
{
	while (1) {
		struct cmdline *l;
		int i, j;//, k;

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

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				/*printf("command of the pipe: \n");*/
				printf("cmd[%d]: %s ", j, cmd[j]);
				/*printf("command of the sequence: \n");
				for (k=0; cmd[j][k]!=0; k++) {
					printf("%s ", cmd[j][k]);
				}*/
			}
			printf("\n");
			cmd_simple(l,cmd);
		}
	}
}