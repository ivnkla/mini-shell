#!/bin/bash
#script permettant d'effectuer tous les tests

#script
for testX in "./tests"/*
do
	echo "Test avec le fichier $testX..."
	echo
	./sdriver.pl -t $testX -s ./shell
	echo
	echo "Fin du test $testX."
	echo
done


#./sdriver.pl -t test01.txt -s ./shell
