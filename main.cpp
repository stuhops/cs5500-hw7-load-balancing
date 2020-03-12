#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <chrono> 

#define MCW MPI_COMM_WORLD

using namespace std;
using namespace std::chrono; 


// TODO: Get dual pass ring termination working
// TODO: Get random send and receive working
// TODO: Auto generate numbers and do auto send and receive

void printTokenData(int rank, int size, int token, int special) {
  cout << "----------------------------------" << endl
       << "Rank    : " << rank << " / " << size-1 << endl
       << "Token   : " << token << endl
       << "Special : " << special << endl
       << "----------------------------------"
       << endl;
}

void print(string toPrint) {
  cout << toPrint << endl;
}


int main(int argc, char **argv) {

  int rank, size;
  int data;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);
  MPI_Status tokenStatus;
  MPI_Status dataStatus;

  int special = 0;
  bool workDone = true;
  int token = 2;  // 2 is white, 1 is black, 0 is terminate
  int tokenStarted = false;

  if(rank == 2) special = 1;

  while(token) {
    // <<<<<<<<<<<<<<  Gather data from other processes >>>>>>>>>>>>>>>>>>>
    // MPI_Probe((rank - 1 + size) % size, 0, MCW, &tokenStatus);


    // <<<<<<<<<<<<<<<  Dual-pass ring termination >>>>>>>>>>>>>>>>>>>>>>>>
    if(workDone) {
      if(!rank){
        if(tokenStarted) {
          // TODO: Receive a message from the last process and OR token with it
          if(token == 2) {
            token = 0;
            printTokenData(rank, size, token, special);
            MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, 0, MCW);
          }
          else {
            token = 2;
            printTokenData(rank, size, token, special);
            MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, 0, MCW);
            MPI_Recv(&token, 1, MPI_INT, (rank - 1 + size) % size, 0, MCW,MPI_STATUS_IGNORE);
          }
        }
        else {
          tokenStarted = true;
          token = 2;
          printTokenData(rank, size, token, special);
          MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, 0, MCW);
          MPI_Recv(&token, 1, MPI_INT, (rank - 1 + size) % size, 0, MCW,MPI_STATUS_IGNORE);
        }
      }
      else {
        // TODO: Receive a message from the last process and OR token with it
        MPI_Recv(&token, 1, MPI_INT, (rank - 1 + size) % size, 0, MCW,MPI_STATUS_IGNORE);
        printTokenData(rank, size, token, special);
        if(token == 0) {
          MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, 0, MCW);
          break;
        }
        else if(token == 2) 
          token = token - special;

        special = 0;
        MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, 0, MCW);
      }
    }

    else {
      // <<<<<<<<<<<<<<<<<<<<<<<< Perform Work >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

      // <<<<<<<<<<<<<<<<<<<<<<< Generate Work >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    }

  }

  // MPI_Send(&data, 1, MPI_INT, dest, 0, MCW);
  // MPI_Recv(&data, 1, MPI_INT, dest, 0, MCW,MPI_STATUS_IGNORE);

  // auto start = high_resolution_clock::now();
  // auto stop = high_resolution_clock::now(); 
  // auto duration = duration_cast<microseconds>(stop - start);
  // cout << " TIME: " << duration.count() << " microseconds" << endl;







  MPI_Finalize();
  return 0;
}