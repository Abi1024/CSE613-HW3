#include <cstdio>
#include <math.h>
#include <vector>
#include <fstream>
#include <mpi.h>
using namespace std;

int** alloc_2d_init(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);
    return array;
}

void print2d_vector(vector<vector<int>>& A, ofstream& out){
  for (unsigned int i = 0; i < A.size(); i++){
    for (unsigned int j = 0; j < A[0].size(); j++){
      out  << A[i][j] << " ";
    }
    out << endl;
  }
}

void setup_array(int** A, int M, int m, int base_x, int base_y){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      A[i][j] = (base_x+i)*M + (base_y+j) + 1;
    }
  }
}

int main(int argc, char* argv[]){
  int ierr = MPI_Init(&argc, &argv);
  int procid, P;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD,&procid);
  ierr = MPI_Comm_size(MPI_COMM_WORLD,&P);

  int p = sqrt(P);
  int M = 4;
  int m = M/p;

  ofstream output;
  output.open("err" + to_string(procid) + ".txt");

  int p_row = procid/p;
  int p_column = procid%p;
  int base_x = p_row*m;
  int base_y = p_column*m;
  //printf("ProcID %d has base_x %d and base_y %d.\n",procid,base_x,base_y);
  printf("ProcID %d has p_row %d and p_column %d.\n",procid,p_row,p_column);
  int **A;
  A = alloc_2d_init(m,m);
  setup_array(A,M,m,base_x,base_y);
  if (p_row != 0){
    if (p_column % 2 == 0){
      //send A
      int receiver = (p_row)*p+((p_column+p-p_row)%p);
      printf("EVEN: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
      MPI_Send(&(A[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
      int sender = (p_row)*p+((p_column+p_row)%p);
      MPI_Status status;
      printf("EVEN: ProcID %d is waiting to hear from sender %d\n", procid, sender);
      ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);

    }else{
      //receive A
      MPI_Status status;
      int sender = (p_row)*p+((p_column+p_row)%p);
      printf("ODD: ProcID %d is waiting to hear from sender %d\n", procid, sender);
      ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
      int **A2;
      A2 = alloc_2d_init(m,m);
      setup_array(A2,M,m,base_x,base_y);
      int receiver = (p_row)*p+((p_column+p-p_row)%p);
      printf("ODD: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
      MPI_Send(&(A2[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
      free(A2[0]);
      free(A2);
    }
    if (ierr == MPI_SUCCESS){
      output << "Processor " << procid << " successfully received!" << endl;
    }else{
      exit(1);
    }
  }
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      output << A[i][j] << " ";
    }
    output << endl;
  }
  free(A[0]);
  free(A);





  //second phase


  ierr = MPI_Finalize();
  output.close();
}
