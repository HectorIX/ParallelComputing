#!/bin/bash
COUNT=2
# bash until loop

until [ $COUNT -gt 64 ]; do

	srun -n $COUNT --mpi=pmi2 ./a.out 1 15000 >> file2.txt  

        let COUNT=COUNT+1
done 
