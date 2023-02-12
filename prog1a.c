#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEED 13242532

int main(int argc, char **argv){
	if (argc != 3){
		printf("Use : a.out <no-of-iterations> <no-of-threads>\n");
		exit(0);
	}
	long niter = atol(argv[1]), count = 0, i;
	double x, y,z, pi;
	int n = atoi(argv[2]);

	srand(SEED);
	
	double t = omp_get_wtime();

	#pragma omp parallel for reduction(+:count) private(i,x,y,z) num_threads(n)
	for (i = 0; i < niter; i++){
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		z = x*x + y*y;
		if (z <= 1.0)
			count++;
	}

	pi = (double) 4 * ((double)count / (double)niter);
	t = omp_get_wtime() - t;
	printf("\napprox pi => %g\nTime => %lf\n",pi,(t));
	return 0;
}
