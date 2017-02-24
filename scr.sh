#!/bin/bash
COUNT=2
# bash until loop

until [ $COUNT -gt 5 ]; do

	srun -n 1 --mpi=pmi2 ./a.out 1 15000 >> opt_serial_15.txt  

        let COUNT=COUNT+1
done 
