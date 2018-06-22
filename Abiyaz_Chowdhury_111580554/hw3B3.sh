#!/bin/bash
#SBATCH --partition=compute    ### Partition
#SBATCH --job-name=HelloMPI  ### Job Name
#SBATCH --time=00:30:00      ### WallTime (10 minutes)
#SBATCH --nodes=16        ### Number of Nodes
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
ibrun --npernode 1 ./a.out
