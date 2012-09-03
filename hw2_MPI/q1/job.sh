#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 48
#$ -N e1
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M rohitchawla88@gmail.com
#$ -m be
#$ -l h_rt=01:00:00


./q1<3dfd-1.in > ./q1_output
