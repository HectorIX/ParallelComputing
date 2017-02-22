#!/bin/bash
COUNT=2
# bash until loop

until [ $COUNT -gt 5 ]; do

	mpirun -n $COUNT ./a.out 1 10 >> file.txt  

        let COUNT=COUNT+1
done 
