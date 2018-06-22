#include <iostream>
#include <fstream>
#include <cstdlib>
#include <omp.h>
#include <vector>
using namespace std;
ofstream output;

int main(){
  output.open("err0.txt");
  omp_set_num_threads(4);
  output << "TEST NUM PROCS222: " << omp_get_num_procs();
  output.close();
}
