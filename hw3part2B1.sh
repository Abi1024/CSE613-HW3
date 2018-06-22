#!/bin/bash
#SBATCH --partition=compute    ### Partition
#SBATCH --job-name=HelloMPI  ### Job Name
#SBATCH --time=00:30:00      ### WallTime (10 minutes)
#SBATCH --nodes=1        ### Number of Nodes
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=24
export OMP_NUM_THREADS=24
ibrun --npernode 1 ./a.out
