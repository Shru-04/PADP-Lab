#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <error.h>
#include <mpi.h>

#define SEED 13124324

int main(int argc, char **argv){
	if (argc != 2){
		error(1,0,"Usage : a.out <no. of iterations>");
		exit(1);
	}

	//Start mpi
	MPI_Init(&argc,&argv);
	double x, y, pi;
	long i, count = 0, sum = 0, niter = atol(argv[1]);
	int rank, numproc;

	MPI_Comm_size(MPI_COMM_WORLD,&numproc);		//no. of processes
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);		//rank (id)
	
	for (i = 0; i < niter; i++){
		x = (double)rand() / RAND_MAX;
		y = (double)rand() / RAND_MAX;
		if ((x*x +y*y) < 1.0)
			count++;
	}

	MPI_Reduce(&count,&sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);		//Reduction (v.imp)

	pi = 4 * (double)sum / ((double)niter * numproc);
	rank == 0 && printf("Approx. value of PI = %lf\n",pi);

	MPI_Finalize(); //terminate
}
