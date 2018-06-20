#include <iostream>
#include <fstream>
#include <cstdlib>
#include <mpi.h>
#include <vector>
using namespace std;

int** alloc_2d_init(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);
    return array;
}

int main(int argc, char* argv[]){
  int size = 5;

  int ierr = MPI_Init(&argc, &argv);
  int procid, P;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD,&procid);
  ierr = MPI_Comm_size(MPI_COMM_WORLD,&P);

  ofstream output;
  output.open("output" + to_string(procid) + ".txt");

  if (procid == 0){
    int **A;
    A = alloc_2d_init(size,size);
    for (int i = 0; i < size; i++){
      for (int j = 0; j < size; j++){
        A[i][j] = 5;
      }
    }
    MPI_Send(&(A[0][0]), size*size, MPI_INT, 1, 0, MPI_COMM_WORLD);
    free(A[0]);
    free(A);
  }else if (procid == 1){
    int **B;
    B = alloc_2d_init(size,size);
    MPI_Status status;
    ierr = MPI_Recv(&(B[0][0]), size*size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    if (ierr == MPI_SUCCESS){
      cout << "Successful receive!" << endl;
      for (int i = 0 ; i < size; i++){
        for (int j = 0; j < size; j++){
          cout << B[i][j] << " ";
        }
        cout << endl;
      }
    }
    free(B[0]);
    free(B);
  }
  ierr = MPI_Finalize();
  output.close();
}
