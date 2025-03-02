# mini-shell

## Objectif
L'idée est de réaliser un shell, qui gère :
* les commandes simples du types :
`ls`, `cat`
* les commandes avec redirections comme :
`ls > toto.txt`, `cat < toto.txt`
* les commandes reliées par un tube comme :
`echo "apple banana carot" | tr apple lemon | sort`
* les processus en arrière plan :
`ps &`
* les processus zombis

## Ce qui fonctionne
* commandes simples ✅
* commandes simples avec redirections ✅
* commandes tubulaires avec redirections ✅
* arrière plan ❌
* zombies ✅

## L'implémentation

`cmd_simple()` gère les commandes sans pipes avec ou sans redirections. En cas de redirection ‘out’ (cmd > file), on ouvre le fichier (ou crée s’il n’existe pas) et on fait pointer la sortie standard `stdout` vers ce fichier. De cette manière, la sortie de la commande sera capturée dans le fichier de sortie. On procède de la même façon pour une redirection ‘in’ (cmd < file). Après avoir traité ces deux cas on peut exécuter la commande avec execvp(). Les descripteurs de `stdin`, et `stdout` n’ont pas besoin d’être restaurés car ils n’ont été modifiés que dans le processus fils, de fait, les descripteurs du père restent inchangés. 

Notons que cette fonction est inutilisée dès l'implémentatation de `execute_pipeline` qui traite ces cas particuliers, mais aussi les pipes. Attention, cette fonction ne gère que les cas du type : `cmd1 < in.txt | cmd2 | cmd3 | ... | cmdN > out.txt` et non les cas farfelus du genre `cmd1 > fichier1.txt | cmd2 | cmd3 | ... | cmdN < fichier2.txt`.

Dans le détail,`execute_pipeline()` utilise une fonction auxiliaire nommée `pip_cmd_simple()` qui prend en argument deux descripteurs de fichiers, un pour lire et l'autre pour écrire. Elle se charge de faire pointer le premier descripteur vers l'entrée standard `stdin` et s'occupe de faire pointer le second descripteur sur la sortie standard `stdout` de façon à créer un pont entre `fildes1` la commande courante `cmd`et `fildes2`. C'est `execute_pipeline()` qui s'occupe d'exécuter toutes les commandes du pipe en prenant en compte les redirections qui peuvent arriver au début et à la fin de la pipeline.

## Comment utiliser le shell ?
### Compiler 
Compiler simplement :
```c
make
```
Pour compiler avec des options de debug :
```c
make debug
```

### Utiliser le shell
Après avoir compiler, lancer :
```c
./shell
```

### Tests 
Pour effectuer une batterie de tests :
```c
./all.sh
```

Notez que tous les tests effectués se trouvent dans le répertoire `tests` et vous pouvez ajoutez vos propres tests moyennant le format suivant :
```bash
# commentaire 1
# commentaire 2
# ...

ici la commande que vous souhaitez tester
la seconde commande que vous souhaitez tester 
etc...
```
### Structure
```sh
.
├── Makefile
├── README.md
├── all.sh
├── lorem.txt
├── notes.txt
├── sdriver.pl
├── src
│   ├── csapp.c
│   ├── csapp.h
│   ├── readcmd.c
│   ├── readcmd.h
│   └── shell.c
└── tests
    ├── test01.txt
    ├── test02.txt
    ├── test03.txt
    ├── test04.txt
    ├── test05.txt
    ├── test06.txt
    ├── test07.txt
    ├── test08.txt
    ├── test09.txt
    └── test10.txt
```
