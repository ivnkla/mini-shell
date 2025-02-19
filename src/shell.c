/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"


int main()
{
	while (1) {
		struct cmdline *l;
		int i, j, k;

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

			/*gestion d'une commande simple*/
        	/*factoriser plus tard dans une fonction a part entière*/
			pid_t pid = fork();
			if (pid==0) { //execute from the child
				printf("hello from the child\n");
				printf("---------------------------------------------------------------------------------------------------------------------------\n");
				if (execvp(cmd[0], &cmd[0])==-1){
					perror("execvp failed\n");
					exit(0);
				}
			}
			waitpid(pid, NULL, 0);
			printf("---------------------------------------------------------------------------------------------------------------------------\n");
			
			/*fin de la commande simple*/
		}
	}
}
