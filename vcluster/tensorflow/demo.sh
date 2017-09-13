#!/bin/bash 
~/benchmark/tools/ssh_copy_id_to_all.sh $1
~/benchmark/tools/genhosts.sh $1
#mpirun -f hosts.txt benchmark/mpi/gemm/gemm 1000 1000 1000
parallel-ssh -h hosts.txt -t 18000 "cd / && ./TFdemo.sh"
