void simplePipe(struct cmdline* l) {
    pid_t pid, pidBis;
    int tube[2];
	//char **cmd = l->seq[0];
    
    // Création d'un tube
    if (pipe(tube) == -1) {  
        fprintf(stderr, "Erreur de création du tube\n");
        exit(1);
    }

    // Création d'un processus fils
    pid = Fork();

    if (pid == -1) {
        fprintf(stderr, "Erreur de fork\n");
        exit(2);
    }

    if (pid == 0) { // Processus fils

		
        // Fermer l'extrémité d'écriture du tube
        Close(tube[1]);
        
        // Rediriger l'entrée standard vers le tube
        Dup2(tube[0], 0);
        
        // Fermer le descripteur du tube
        Close(tube[0]);

        // Exécuter la deuxième commande (là où l'entrée est redirigée)
        execvp(l->seq[1][0], l->seq[1]);
		//cmd = l->seq[0];
		//cmd_simple(l,cmd);
        
        // Si execvp échoue
        perror("Erreur execvp");
        exit(3);
    }
	pidBis = Fork();
	
	if (pidBis == -1) {
        fprintf(stderr, "Erreur de fork\n");
        exit(2);
    }
	if (pidBis == 0){
		// Fermer l'extrémité de lecture du tube
        Close(tube[0]);

        // Rediriger la sortie standard vers le tube
        Dup2(tube[1], 1);  // 1 représente STDOUT_FILENO

        // Fermer le descripteur du tube
        Close(tube[1]);

        // Exécuter la première commande (là où la sortie est redirigée)
        execvp(l->seq[0][0], l->seq[0]);
		/*cmd = l->seq[1];
		cmd_simple(l,cmd);*/

        // Si execvp échoue
        perror("Erreur execvp");
        exit(4);
	}
	else { // Processus parent
	
		// Attendre la fin des deux processus enfants
		waitpid(pid, NULL, 0);
		waitpid(pidBis, NULL, 0);
    }
}
