#!/bin/bash
COUNT=2
# bash until loop

until [ $COUNT -gt 12 ]; do

	srun -n $COUNT --mpi=pmi2 ./a.out 1 100000 >> 100k_part1.txt  

        let COUNT=COUNT+1
done 
