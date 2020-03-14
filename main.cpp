#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <bits/stdc++.h> 
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
  cout << "Rank    : " << rank << " / " << size-1
       << "      Token   : " << token
       << "      Special : " << special
       << endl;
  sleep(1);
}
void printTokenData(int rank, int size, int token, int special, int cnt) {
  cout << "Rank    : " << rank << " / " << size-1 
       << "    Token   : " << token 
       << "    Special : " << special 
       << "    Loop Cnt: " << cnt 
       << endl;
  sleep(1);
}

void print(string toPrint) {
  cout << toPrint << endl;
}
void print(string toPrint, int pInt) {
  cout << toPrint << ": " << pInt << endl;
}
void print(int rank, string toPrint, int pInt) {
  cout << "Rank " << rank << " " << toPrint << ": " << pInt << endl;
}


int main(int argc, char **argv) {

  int rank, size;
  int data;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);
  MPI_Status token_status;
  MPI_Status work_status;

  const int TOKEN_TAG = 0;
  const int WORK_TAG = 1;

  stack <int> work;
  int incoming_work[2];
  // const int WORK_TO_GENERATE = rand() % 1024 + 1024;
  const int WORK_TO_GENERATE = 2;
  const int WORK_THRESHHOLD = 16;

  int special = 0;
  int token = 2;  // 2 is white, 1 is black, 0 is terminate
  int tokenStarted = false;
  
  int token_flag;
  int work_flag;

  if(rank == 2) special = 1;
  if(rank == 1) special = 1;

  // <<<<<<<<<<<<<<<<<<<<<<< TEMPORARY >>>>>>>>>>>>>>>>>>>>>>>
  if(!rank) {
    work.push(1);
  }

  while(token) {
    // <<<<<<<<<<<<<<  Gather data from other processes >>>>>>>>>>>>>>>>>>>
    token_flag = 0;
    MPI_Iprobe((rank - 1 + size) % size, TOKEN_TAG, MCW, &token_flag, &token_status);
    if(token_flag) {
      MPI_Recv(&token, 1, MPI_INT, (rank - 1 + size) % size, TOKEN_TAG, MCW,MPI_STATUS_IGNORE);
    }

    for(int i = 0; i < size; i++) {
      work_flag = 0;
      MPI_Iprobe(i, WORK_TAG, MCW, &work_flag, &work_status);
      if(work_flag) {
        int count;
        MPI_Get_count(&work_status, MPI_INT, &count);

        MPI_Recv(&incoming_work, count, MPI_INT, (rank - 1 + size) % size, TOKEN_TAG, MCW,MPI_STATUS_IGNORE);
        for(int j = 0; j < count; j++) {
          work.push(incoming_work[j]);
        }
      }
    }

    // <<<<<<<<<<<<<<<<<<<<<< Send excess work out >>>>>>>>>>>>>>>>>>>>>>>>>>

    // <<<<<<<<<<<<<<<<<<  Dual-pass ring termination >>>>>>>>>>>>>>>>>>>>>>>
    if(work.empty()) {
      if(!rank){
        if(tokenStarted) {
          if(token_flag) {
            printTokenData(rank, size, token, special);
            if(token == 2) {
              token = 0;
              MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MCW);
            }
            else {
              token = 2;
              MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MCW);
            }
          }
        }
        else {
          tokenStarted = true;
          token = 2;
          printTokenData(rank, size, token, special);
          MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MCW);
        }
      }
      else if(token_flag) {
        printTokenData(rank, size, token, special);

        if(token == 0) {
          MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MCW);
          break;
        }
        else if(token == 2) 
          token = token - special;

        special = 0;
        MPI_Send(&token, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MCW);
      }

      token_flag = 0;
    }

    else {
      // <<<<<<<<<<<<<<<<<<<<<<<< Perform Work >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      for(int i = 0; !work.empty() && i < 10; i++) {
        int work_item = work.top();
        work.pop();
        print(rank, "Stack Size", work.size());
        print(rank, "Start Work Item", work_item);
        int tmp = work_item;
        for(int j = 0; j < work_item; j++) {
          for(int k = 0; k < work_item; k++) {
            tmp++;
          }
        }
        work_item = tmp;
      }

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