#!/bin/bash
#SBATCH --partition=compute    ### Partition
#SBATCH --job-name=HelloMPI  ### Job Name
#SBATCH --time=00:30:00      ### WallTime (10 minutes)
#SBATCH --nodes=1        ### Number of Nodes
#SBATCH --ntasks-per-node=16

ibrun --npernode 16 ./a.out
