#include <iostream>
#include <fstream>
#include <cstdlib>
#include <mpi.h>
#include <vector>
using namespace std;
ofstream output;

int** alloc_2d_init(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);
    return array;
}

void print2d_vector(int** A, int size){
  for (int i = 0; i < size; i++){
    for (int j = 0; j < size; j++){
      output  << A[i][j] << " ";
    }
    output << endl;
  }
}

int main(int argc, char* argv[]){
  int size = 5;

  int ierr = MPI_Init(&argc, &argv);
  int procid, P;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD,&procid);
  ierr = MPI_Comm_size(MPI_COMM_WORLD,&P);

  output.open("err" + to_string(procid) + ".txt");

  int **A;
  A = alloc_2d_init(size,size);
  if (procid == 0){
    for (int i = 0; i < size; i++){
      for (int j = 0; j < size; j++){
        A[i][j] = 5;
      }
    }
  }


  ierr = MPI_Bcast(&(A[0][0]),size*size,MPI_INT,0,MPI_COMM_WORLD);
  print2d_vector(A,size);


  free(A[0]);
  free(A);


  ierr = MPI_Finalize();
  output.close();
}
