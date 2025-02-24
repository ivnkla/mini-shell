# mini-shell

##Objectif
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

##Ce qui fonctionne
* commandes simples ✅
* commandes simples avec redirections ✅
* commandes tubulaires avec redirections ✅
* arrière plan ✅
* zombies ✅

##L'implémentation


