/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

 /* NOTE : You'll find a detailed explanation of the functions in the repo https://github.com/ivnkla/mini-shell in the EREADME file*/

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <stdbool.h>

//adding a debug option, pas sûr de l'utilité...
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "[DEBUG] " fmt "\n", ##args)
#else
    #define DEBUG_PRINT(fmt, args...)
#endif

/*USELESS CAUSE HANDLED IN THE PIPELINE METHOD*/
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
/*REMINDER OF USELESSNESS OF THIS FUNCTION AS HANDLED IN THE PIPELINE METHOD*/

//just do a single cmd and read an input (or write an ouput) if need, we keep it simple here
void pipe_cmd_simple(struct cmdline* l, char **cmd, int read_fd, int write_fd) {
    pid_t pid;

    if ((pid = Fork()) == 0) {
        if (read_fd != -1) {
            Dup2(read_fd, STDIN_FILENO); //make point STDIN into read_fd
            Close(read_fd);
        }
        if (write_fd != -1) {
            Dup2(write_fd, STDOUT_FILENO); //make point STDOUT into write_fd
            Close(write_fd);
        }
        execvp(cmd[0], cmd);
        perror("execvp error");
        exit(EXIT_FAILURE);
    }
	waitpid(pid, NULL, 0);
}

//manage the whole pipeline here
void execute_pipeline(struct cmdline* l) {
    int fd[2];
    int prev_read_fd = -1;
	int last_write_fd = -1;

	//dealing with input and output files respectively only for the first and last command
	if (l->in) {
		//we attribute the fildes to the prev_read_fd
		if ((prev_read_fd = Open(l->in, O_RDONLY, 0444)) == -1) {
			perror("in opening error");
			exit(EXIT_FAILURE);
		}
	}
	if (l->out) {
		if ((last_write_fd = Open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
			perror("out opening error");
			exit(EXIT_FAILURE);
 		}
	}

    int i = 0;
    while (l->seq[i] != NULL) {
        if (l->seq[i + 1] != NULL) { //if there is following commands
            pipe(fd); //create a pipe for that pair
            pipe_cmd_simple(l, l->seq[i], prev_read_fd, fd[1]); //write the output of cmd1 into the pipe taking prev_read_fd as an input
            Close(fd[1]);
            if (prev_read_fd != -1) {
				Close(prev_read_fd);
			}
            prev_read_fd = fd[0]; //keeping that new read for the next command
        } else { //last command
            pipe_cmd_simple(l, l->seq[i], prev_read_fd, last_write_fd);
            if (prev_read_fd != -1) {
				Close(prev_read_fd);
			}
			if (last_write_fd != -1) {
				Close(last_write_fd);
			}
        }
        i++;
    }
	
	//dirty way to do that buttt, idk if even usefull
    //while (wait(NULL) > 0);
}

/*Traitement des zombis*/

void handlerZ(){
	pid_t pid;
	int status;
	//printf("on est avant la boucle");
	while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0){
		//fprintf(stdout,"on attend ici la fin des fils, ");
		//fprintf(stdout,"status : %d\n",status);
	}

	//fprintf(stdout,"status : %d\n",status);
	return;
}

int main()
{
	Signal(SIGCHLD,handlerZ);	//traitant du SIGCHLD
	while (1) {
		struct cmdline *l;

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
		execute_pipeline(l);
	}
}
