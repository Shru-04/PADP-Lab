#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	long *m1, *m2, *res;
	long r1, c1, r2, c2;
	double t;
	if (argc != 3){
		printf("Usage : a.out <matrix-size> <num-of-threads>\n");
		exit(1);
	}

	r1 = c1 = r2 = c2 = atol(argv[1]);

	m1 = (long *) malloc (r1 * c1 * sizeof(long));
	m2 = (long *) malloc (r2 * c2 * sizeof(long));
	res = (long *) malloc (r1 * c2 * sizeof(long));

	//Allocate
	for (long i = 0; i < r1; i++) for (long j = 0; j < c1; j++) m1[i * c1 + j] = i + j;
	for (long i = 0; i < r2; i++) for (long j = 0; j < c2; j++) m2[i * c2 + j] = i + j;
	for (long i = 0; i < r1; i++) for (long j = 0; j < c2; j++) res[i * c2 + j] = 0;

	t = omp_get_wtime();
	
	// Use parallel here
	#pragma omp parallel for num_threads(atoi(argv[2]))
	for (long i = 0; i < r1; i++)
		for (long j = 0; j < c2; j++)
			for (long k = 0; k < r2; k++)
				res[c2 * i + j] += m1[c1 * i + k] * m2[c2 * k + j];
	t = omp_get_wtime() - t;

	//printf("Result Matrix :\n");
	//for (long i = 0; i < r1; i++){ for (long j = 0; j < c2; j++) printf("%ld ",res[i * c2 + j]); printf("\n");}

	printf("\nTime taken => %lf\n",t);
	return 0;
}
