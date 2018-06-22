#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <fstream>
#include <mpi.h>
using namespace std;
ofstream output;
ofstream csv;

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

void setup_array(int** A,int m){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      A[i][j] = (rand()%201)-100;
    }
  }
}

void setup_array_A(int** A, int M, int m, int base_x, int base_y){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      A[i][j] = (base_x+i)*M + (base_y+j);
    }
  }
}

void setup_array_B(int** B, int M, int m, int base_x, int base_y){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      B[i][j] = (base_x+i)*M + (base_y+j);
    }
  }
}

void setup_array_C(int** C, int m){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      C[i][j] = 0;
    }
  }
}

void copy_array(int** source, int** dest, int m){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      dest[i][j] = source[i][j];
    }
  }
}

double run(int M,int argc, char* argv[], int procid, int P){
  int p = sqrt(P);
  int m = M/p;

  int p_row = procid/p;
  int p_column = procid%p;
  int base_x = p_row*m;
  int base_y = p_column*m;
  //printf("ProcID %d has base_x %d and base_y %d.\n",procid,base_x,base_y);
  //printf("ProcID %d has p_row %d and p_column %d.\n",procid,p_row,p_column);

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time =  MPI_Wtime();

  int **A;
  A = alloc_2d_init(m,m);
  setup_array(A,m);
  //setup_array_A(A,M,m,base_x,base_y);
  //=========================================================================================================================================
  int **B;
  B = alloc_2d_init(m,m);
  setup_array(B,m);
  //setup_array_B(B,M,m,base_x,base_y);
  //====================================================================================
  int **C;
  C = alloc_2d_init(m,m);
  setup_array_C(C,m);

  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, p_row, P, &row_comm);
  int row_rank;
  MPI_Comm_rank(row_comm, &row_rank);

  MPI_Comm col_comm;
  MPI_Comm_split(MPI_COMM_WORLD, p_column, P, &col_comm);
  int col_rank;
  MPI_Comm_rank(col_comm, &col_rank);

  int **A2;
  A2 = alloc_2d_init(m,m);
  int **B2;
  B2 = alloc_2d_init(m,m);

  for (int l = 0; l < p; l++){

    output << "Iteration: " << l << endl;
    copy_array(A,A2,m);
    MPI_Bcast(&(A2[0][0]),m*m,MPI_INT,l,row_comm);
    copy_array(B,B2,m);
    MPI_Bcast(&(B2[0][0]),m*m,MPI_INT,l,col_comm);
    //output << "Printing A2:" << endl;
    //print2d_vector(A2,m);
    //output << "Printing B2:" << endl;
    //print2d_vector(B2,m);
    for (int i = 0; i < m; i++){
      for (int k = 0; k < m; k++){
        for (int j = 0; j < m; j++){
          C[i][j] += A2[i][k]*B2[k][j];
        }
      }
    }



  }

  MPI_Comm_free(&row_comm);
  MPI_Comm_free(&col_comm);
  output << "Printing C" << endl;
  //print2d_vector(C,m);

  MPI_Barrier(MPI_COMM_WORLD);
  double time = MPI_Wtime() - start_time;


  free(C[0]);
  free(C);
  free(A[0]);
  free(A);
  free(B[0]);
  free(B);
  free(A2[0]);
  free(A2);
  free(B2[0]);
  free(B2);

  return time;
}

int main(int argc, char* argv[]){
  srand (time(NULL));
  int ierr = MPI_Init(&argc, &argv);
  int procid, P;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD,&procid);
  ierr = MPI_Comm_size(MPI_COMM_WORLD,&P);
  output.open("err" + to_string(procid) + ".txt");
  output << "COMM SIZE: " << P << endl;
  csv.open("hw3" + to_string(procid) + ".csv");
  for (int i = 0; i < 5; i++){
    printf("Iteration with procid: %d\n",procid);
    int M = pow(2,10+i);
    double time = run(M, argc, argv, procid, P);
    csv << M << "," << time << endl;
  }
  ierr = MPI_Finalize();
  output.close();
  csv.close();
}
