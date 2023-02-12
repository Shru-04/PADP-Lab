#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define BUF_SIZE 32

int main(int argc, char **argv){
	int numprocs, rank, i;
	int root = 0, tag = 0;
	char *messages[] = {"HELLO", "RVCE", "CSE"};
	char msg[BUF_SIZE];  //imp to alloc buf size XD
	MPI_Init(&argc, &argv);					//init
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);		//get no. of processes alloc
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);			//get rank
	if (rank != 0){
		strcpy(msg,messages[rank-1]);
		MPI_Send(msg,BUF_SIZE,MPI_CHAR,root,tag,MPI_COMM_WORLD);
	}
	else{
		for (i = 1; i < numprocs; i++){
			MPI_Recv(msg,BUF_SIZE,MPI_CHAR,i,tag,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			printf("Process of rank %d sent a msg to root process of rank %d : %s\n",i,root,msg);
		}
	}
	MPI_Finalize();
	return 0;
}
