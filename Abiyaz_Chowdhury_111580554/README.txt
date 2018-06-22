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

Task 1, D, E and F (use the same shell scripts as from the above parts):
mpic++ -std=c++11 -g hw3part1D.cpp

Task 2, A and B (the first cpp file is OpenMP modification of task 1A, second cpp file is modification of 1D)(Each job script corresponds to 1,4 and 16 nodes respectively).
mpic++ -std=c++11 -g hw3part2.cpp
mpic++ -std=c++11 -g hw3part2_type2.cpp
sbatch hw3part2B1.sh
sbatch hw3part2B2.sh
sbatch hw3part2B3.sh