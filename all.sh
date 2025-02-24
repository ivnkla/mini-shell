#!/bin/bash
#script permettant d'effectuer tous les tests

#script
for testX in "./tests"/*
do
	echo "Test avec le fichier $testX..."
	./sdriver.pl -t $testX -s ./shell
done


#./sdriver.pl -t test01.txt -s ./shell
