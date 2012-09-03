#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 1way 48
#$ -N q2
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M rohitchawla88@gmail.com
#$ -m be
#$ -l h_rt=01:00:00


mpirun -np 3 ./a.out<3dfd-1.in > ./q4mpi_output

