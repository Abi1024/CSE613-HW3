Abiyaz Chowdhury, 2018
This document describes how to run the programs implemented in HW3.

Task 1, A and B (each cpp file is one of the implementations and each shell script is either 1,4 or 16 nodes):
mpic++ -std=c++11 -g hw3.cpp
mpic++ -std=c++11 -g hw3_type2.cpp
mpic++ -std=c++11 -g hw3_type3.cpp

sbatch hw3B1.sh
sbatch hw3B2.sh
sbatch hw3B3.sh

Task 1, C (run each batch script after compiling one of the implementations as shown above):
sbatch hw3C1.sh
sbatch hw3C2.sh
sbatch hw3C3.sh

Task 1, D and E:
mpic++ -std=c++11 -g hw3part1D.cpp
sbatch hw3B.sh