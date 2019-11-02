#!/bin/bash

counter=1

while [ $counter -le 1024 ]
do
	VAR1="A$counter"
	mkdir $VAR1
	((counter++))
done 
