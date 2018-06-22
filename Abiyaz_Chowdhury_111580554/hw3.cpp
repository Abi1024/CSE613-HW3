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

int gcd(int a, int b)
{
    // Everything divides 0
    if (a == 0 || b == 0)
       return 0;

    // base case
    if (a == b)
        return a;

    // a is greater
    if (a > b)
        return gcd(a-b, b);
    return gcd(a, b-a);
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

void setup_array(int** A,int m){
  for (int i = 0 ; i < m; i++){
    for (int j = 0; j < m; j++){
      A[i][j] = (rand()%201)-100;
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
  int factor = 0;
  if (x != 1){
    factor = gcd(p_row,m);
    //output << "Factor: " << factor << endl;
    //output << "pcolumn: " << p_column << endl;
  }
  if (((x == 1) && (p_column % 2 == 0))||((x != 1)&&(p_column % (factor*2)) < factor)){
    //send A
    int receiver = (p_row)*p+((p_column+p-x)%p);
    //printf("EVEN: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    //output << "EVEN: Waiting to send to: " << receiver << endl;
    MPI_Send(&(A[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    //output << "EVEN: Sent!" << endl;
    int sender = (p_row)*p+((p_column+x)%p);
    MPI_Status status;
    //printf("EVEN: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    //output << "EVEN: Waiting to receive from: " << sender << endl;
    ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    //output << "EVEN: Received!" << endl;
  }else{
    //receive A
    int **A2;
    A2 = alloc_2d_init(m,m);
    copy_array(A,A2,m);
    MPI_Status status;
    int sender = (p_row)*p+((p_column+x)%p);
    //printf("ODD: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    //output << "ODD: Waiting to receive from: " << sender << endl;
    ierr = MPI_Recv(&(A[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    //output << "ODD: Received!" << endl;
    int receiver = (p_row)*p+((p_column+p-x)%p);
    //printf("ODD: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    //output << "ODD: Waiting to send to: " << receiver << endl;
    MPI_Send(&(A2[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    //output << "ODD: Sent!" << endl;
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
  int factor = 0;
  if (x != 1){
    factor = gcd(p_column,m);
    //output << "Factor: " << factor << endl;
    //output << "(1 << factor) "  << (1<<factor) << endl;
    //output << "(1 << (factor-1)) "  << (1<<(factor-1)) << endl;
  }
  if (((x == 1) && (p_row % 2 == 0))||((x != 1)&&(p_row % (factor*2)) < factor) ){
    //send B
    int receiver = ((p_row+p-x)%p)*p+((p_column)%p);
    //printf("EVEN: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    //output << "EVEN: Waiting to send to: " << receiver << endl;
    MPI_Send(&(B[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    //output << "EVEN: Sent!" << endl;
    int sender = ((p_row+x)%p)*p+((p_column)%p);
    MPI_Status status;
    //printf("EVEN: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    //output << "EVEN: Waiting to receive from: " << sender << endl;
    ierr = MPI_Recv(&(B[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    //output << "EVEN: Received!" << endl;
  }else{
    //receive B
    int **B2;
    B2 = alloc_2d_init(m,m);
    copy_array(B,B2,m);
      MPI_Status status;
    int sender = ((p_row+x)%p)*p+((p_column)%p);
    //printf("ODD: ProcID %d is waiting to hear from sender %d\n", procid, sender);
    //output << "ODD: Waiting to receive from: " << sender << endl;
    ierr = MPI_Recv(&(B[0][0]), m*m, MPI_INT, sender, 0, MPI_COMM_WORLD, &status);
    //output << "ODD: Received!" << endl;
    int receiver = ((p_row+p-x)%p)*p+((p_column)%p);
    //printf("ODD: ProcID %d is waiting to send to receiver %d\n", procid, receiver);
    //output << "ODD: Waiting to send to: " << receiver << endl;
    MPI_Send(&(B2[0][0]), m*m, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    //output << "ODD: Sent!" << endl;
    free(B2[0]);
    free(B2);
  }
  if (ierr == MPI_SUCCESS){
    output << "ProcessorB " << procid << " successfully received!" << endl;
  }else{
    exit(1);
  }
}

double run(int M,int argc, char* argv[], int procid, int P){
  //printf("Procid1 %d\n", procid);
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

  //printf("Procid2 %d\n", procid);
  int **A;
  A = alloc_2d_init(m,m);
  setup_array(A,m);
  //setup_array_A(A,M,m,base_x,base_y);
  //output << "PRINTING A: " << endl;
  //print2d_vector(A,m);
  //printf("Yuppers %d\n", procid);
  //output << "ROTATING" << endl;
  if (p_row != 0){
    rotate_left(A,p_row,p_column,procid,base_x,base_y,p,M,m,p_row);
  }
  //print2d_vector(A,m);
  //=========================================================================================================================================
  int **B;
  B = alloc_2d_init(m,m);
  setup_array(B,m);
  //setup_array_B(B,M,m,base_x,base_y);
  //output << "PRINTING VECTOR B" << endl;
  //print2d_vector(B,m);
  //output << "ROTATING B" << endl;
  if (p_column != 0){
    rotate_up(B,p_row,p_column,procid,base_x,base_y,p,M,m,p_column);
  }
  //print2d_vector(B,m);
  //====================================================================================
  int **C;
  C = alloc_2d_init(m,m);
  setup_array_C(C,m);

  //printf("Procid3 %d\n", procid);
  for (int l = 0; l < p; l++){
    //output << "Iteration: " << l << endl;
    for (int i = 0; i < m; i++){
      for (int k = 0; k < m; k++){
        for (int j = 0; j < m; j++){
          C[i][j] += A[i][k]*B[k][j];
        }
      }
    }
    //output << "Doing rotations " << endl;
    if (l < p-1){
      //output << "Rotating left " << endl;
      rotate_left(A,p_row,p_column,procid,base_x,base_y,p,M,m,1);
      //output << "Rotating up " << endl;
      rotate_up(B,p_row,p_column,procid,base_x,base_y,p,M,m,1);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double time = MPI_Wtime() - start_time;
  //output << "Printing C" << endl;
  //print2d_vector(C,m);

  free(C[0]);
  free(C);
  free(A[0]);
  free(A);
  free(B[0]);
  free(B);
  //second phase

  return time;
}

int main(int argc, char* argv[]){
  srand (time(NULL));
  int ierr = MPI_Init(&argc, &argv);
  int procid, P;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD,&procid);
  ierr = MPI_Comm_size(MPI_COMM_WORLD,&P);
  output.open("err" + to_string(procid) + ".txt");
  //output << "COMM SIZE: " << P << endl;
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
