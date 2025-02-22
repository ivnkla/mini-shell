/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

 /* You'll find a detailed explanation of the functions in the repo https://github.com/ivnkla/mini-shell in the EREADME file*/

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"

//adding a debug option, pas sûr de l'utilité...
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "[DEBUG] " fmt "\n", ##args)
#else
    #define DEBUG_PRINT(fmt, args...)
#endif


/*part3 et 4, simple command and redirections*/
void cmd_simple(struct cmdline* l,char **cmd) {
	pid_t pid;
	int in, out = -1;
	if ((pid=Fork())==0) {
		//test if there is a redirection OUT
		if (l->out){
			if ((out = Open(l->out, O_CREAT | O_WRONLY | O_TRUNC , 0644))==-1) {
				perror("Open 'out' error ");
				exit(EXIT_FAILURE);
			}
			DEBUG_PRINT("Debug - hello there is a redirection OUT here");
			Dup2(out,1);//on fait pointer la sortie standard vers le fichier OUT
			Close(out);
		} 
		// test if there is a redirection IN
		if (l->in){		
			if ((in = Open(l->in, O_RDONLY, 0444))==-1) {
				perror("Open 'in' error ");
				exit(EXIT_FAILURE);
			}
			DEBUG_PRINT("Debug - hello there is a redirection IN here");
			Dup2(in,0);	//on fait pointer l'entree standard vers le fichier IN
			Close(in);
		}
		execvp(cmd[0], &cmd[0]);
		//goes here iff execvp failed
		switch(errno) {
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
		exit(EXIT_FAILURE);
	}
	else { //father
		waitpid(pid, NULL, 0);
	}
}

/* part 6 one pipe with (or without) redirection */
void pipe_cmd_simple(struct cmdline* l, char **cmd1, char **cmd2) {
    int fd[2];
    pid_t pid1, pid2;
    int in_fd, out_fd = -1;
    
    if (pipe(fd) == -1) {
        perror("Pipe error ");
        exit(EXIT_FAILURE);
    }
    
    if ((pid1 = Fork()) == -1) {
        perror("Fork error ");
        exit(EXIT_FAILURE);
    }
 
 	//handling cmd1 with first child
    if (pid1 == 0) {
        //if input redirection, make point stdin to that input file 
        if (l->in) {
            in_fd = Open(l->in, O_RDONLY, 0444);
            if (in_fd == -1) {
                perror("Open input file error");
                exit(EXIT_FAILURE);
            }
            Dup2(in_fd, STDIN_FILENO);
            Close(in_fd);
        }
        
        //and then, in any case, write the result into the pipe
        Dup2(fd[1], STDOUT_FILENO);//making point stdout into the pipe
        Close(fd[0]);
        Close(fd[1]);
        execvp(cmd1[0], cmd1);
        perror("execvp error cmd1");
        exit(EXIT_FAILURE);
    }
	
	//by the same way, handling cm2 with second child
    if ((pid2 = Fork()) == -1) {
        perror("Fork error ");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
		//we immediatly make point stdin to the pipe, child2 has to read the result from cmd1
        Dup2(fd[0], STDIN_FILENO);
        //treat the case of an 'output' redirection, similar than before
        if (l->out) {
            out_fd = Open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1) {
                perror("Open output file error");
                exit(EXIT_FAILURE);
            }
            Dup2(out_fd, STDOUT_FILENO);
            Close(out_fd);
        }
        
        Close(fd[0]);
        Close(fd[1]);
        execvp(cmd2[0], cmd2);
        perror("execvp error cmd2");
        exit(EXIT_FAILURE);
    }

    //finish the process from the parent
    Close(fd[0]);
    Close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
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
		
		if (l->in) DEBUG_PRINT("in: %s", l->in);
		if (l->out) DEBUG_PRINT("out: %s", l->out);

		/* Execute each command of the pipe */
		//for (i=0; l->seq[i]!=0; i++) {
			i = 0;
			char **cmd1 = l->seq[i];
			if (l->seq[i+1]!=0) {
				char **cmd2 = l->seq[i+1];
				pipe_cmd_simple(l,cmd1, cmd2);
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
