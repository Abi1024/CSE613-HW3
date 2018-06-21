#include <cstdio>
#include <math.h>
#include <fstream>
#include <mpi.h>
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

void rotate_left(int** A, int p_row, int p_column, int procid, int base_x, int base_y, int p, int M, int m, int x){
  int ierr = 0;
  if (p_column % 2 == 0){
    //send A
    int receiver = (p_row)*p+((p_column+p-x)%p);
    printf("EVEN: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    MPI_Send(&(A[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    int sender = (p_row)*p+((p_column+x)%p);
    MPI_Status status;
    printf("EVEN: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
  }else{
    //receive A
    int **A2;
    A2 = alloc_2d_init(m,m);
    copy_array(A,A2,m);
    MPI_Status status;
    int sender = (p_row)*p+((p_column+x)%p);
    printf("ODD: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    int receiver = (p_row)*p+((p_column+p-x)%p);
    printf("ODD: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    MPI_Send(&(A2[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    free(A2[0]);
    free(A2);
  }
  if (ierr == MPI_SUCCESS){
    output << "ProcessorA " << procid << " successfully received!" << endl;
  }else{
    printf("ProcID %d failed!\n", procid);
    exit(1);
  }
}

void rotate_up(int** B, int p_row, int p_column, int procid, int base_x, int base_y, int p, int M, int m, int x){
  int ierr = 0;
  if (p_row % 2 == 0){
    //send B
    int receiver = ((p_row+p-x)%p)*p+((p_column)%p);
    //printf("EVEN: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    MPI_Send(&(B[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    int sender = ((p_row+x)%p)*p+((p_column)%p);
    MPI_Status status;
    //printf("EVEN: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    ierr = MPI_Recv(&(B[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
  }else{
    //receive B
    int **B2;
    B2 = alloc_2d_init(m,m);
    copy_array(B,B2,m);
      MPI_Status status;
    int sender = ((p_row+x)%p)*p+((p_column)%p);
    //printf("ODD: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    ierr = MPI_Recv(&(B[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    int receiver = ((p_row+p-x)%p)*p+((p_column)%p);
    //printf("ODD: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    MPI_Send(&(B2[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    free(B2[0]);
    free(B2);
  }
  if (ierr == MPI_SUCCESS){
    output << "ProcessorB " << procid << " successfully received!" << endl;
  }else{
    exit(1);
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

  output.open("err" + to_string(procid) + ".txt");

  int p_row = procid/p;
  int p_column = procid%p;
  int base_x = p_row*m;
  int base_y = p_column*m;
  //printf("ProcID %d has base_x %d and base_y %d.\n",procid,base_x,base_y);
  //printf("ProcID %d has p_row %d and p_column %d.\n",procid,p_row,p_column);
  int **A;
  A = alloc_2d_init(m,m);
  setup_array_A(A,M,m,base_x,base_y);
  //=========================================================================================================================================
  int **B;
  B = alloc_2d_init(m,m);
  setup_array_B(B,M,m,base_x,base_y);

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

  int **B2;
  B2 = alloc_2d_init(m,m);

  for (int l = 0; l < p; l++){
    output << "Iteration: " << l << endl;
    copy_array(B,B2,m);
    MPI_Bcast(&(B2[0][0]),m*m,MPI_INT,(l+p_column)%p,col_comm);
    output << "Printing B2:" << endl;
    print2d_vector(B2,m);
    for (int i = 0; i < m; i++){
      for (int j = 0; j < m; j++){
        for (int k = 0; k < m; k++){
          C[i][j] += A[i][k]*B2[k][j];
        }
      }
    }
    if (l < p-1){
      rotate_left(A,p_row,p_column,procid,base_x,base_y,p,M,m,1);
    }

  }

  MPI_Comm_free(&row_comm);
  MPI_Comm_free(&col_comm);
  output << "Printing C" << endl;
  print2d_vector(C,m);

  free(C[0]);
  free(C);
  free(A[0]);
  free(A);
  free(B[0]);
  free(B);
  free(B2[0]);
  free(B2);



  //second phase


  ierr = MPI_Finalize();
  output.close();
}
