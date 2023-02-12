#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>
using namespace std;
double t = 0.0;

//To strike off composite no.s
inline long Strike(bool *composite, long i, long stride, long limit){
	for (; i <= limit; i+=stride)
		composite[i] = true;
	return i;
}

long CacheUnfriendlySieve(long n){
	long count = 0;
	long m = (long)sqrt((double)n);

	// Init composite
	bool *composite = new bool[n+1];
	memset(composite,0,n);

	//Logic
	t = omp_get_wtime();
	for (long i = 2; i <= m; i++){
		if (!composite[i]){
			count++;
			// Strike off multiples of i
			Strike(composite,2*i,i,n);		//limit is n
		}
	}

	for (long i = m+1; i <= n; i++)
		if (!composite[i])
			count++;
	t = omp_get_wtime() - t;

	delete []composite;
	return count;
}

long CacheFriendlySieve(long n){
	long count = 0;
	long n_fac = 0,limit;
	long m = (long)sqrt((double)n);

	// Init composite, striker, factor
	bool *composite = new bool[n+1];
	long *striker = new long[n];		//To store the next composite number to strike off
	long *factor = new long[n];		//To store factor of striker (stride)
	memset(composite,0,n);

	//Logic
	t = omp_get_wtime();
	//till sqrt(n))
	for (long i = 2; i <= m; i++){
		if (!composite[i]){
			count++;
			striker[n_fac] = Strike(composite,2*i,i,m);		//limit is m
			factor[n_fac++] = i;
		}
	}

	//For each m-sized segment of segmented sieve
	for (long window = m+1; window <= n; window+=m){
		//Set Limit
		limit = min(window + m - 1, n);
		
		//Strike composites
		for (long i = 0; i < n_fac; i++)
			striker[i] = Strike(composite,striker[i],factor[i], limit);

		//Count
		for (long i = window; i <= limit; i++)
			if (!composite[i])
				count++;
	}
	t = omp_get_wtime() - t;

	delete []composite;
	delete []striker;
	delete []factor;
	return count;
}

long CacheParallelSieve(long n){
	long count = 0;
	long m = (long)sqrt((double)n);
	long n_fac = 0,limit;
	long *factor = new long[m];
	
	//Logic
	t = omp_get_wtime();
	//Till m
	{
		bool *composite = new bool[m+1];
		memset(composite,0,m);
		for (long i = 2; i <= m; i++){
			if (!composite[i]){
				count++;
				Strike(composite,2*i,i,m);		//limit is m
				factor[n_fac++] = i;
			}
		}
	}

	#pragma omp parallel
	{
		//Init composite,striker,factor for each thread
		bool *composite = new bool[m+1];
		long *striker = new long[m];
		memset(composite,0,m);

		int base = -1;

		//Using segemented sieve

		#pragma omp for reduction(+:count)
		for (long window = m+1; window <= n; window += m){
			memset(composite,0,m);
			
			//Recompute Strikers if not found
			if (base != window){
				for (long i = 0; i < n_fac; i++)
					striker[i] = (window + factor[i] - 1) / factor[i] * factor[i] - window;					//V.V.IMp
			}

			//If strikers r present, proceed
			limit = min(window + m - 1, n) - window;
			for (long i = 0; i < n_fac; i++)
				striker[i] = Strike(composite,striker[i],factor[i],limit) - m;

			for (long i = 0; i <= limit; i++)
				if (!composite[i]){
					count++;
				}

			//Update base
			base += m;
		}
	}
	t = omp_get_wtime() - t;
	return count;
}	


int main(){
	long num;
	cout<<"Enter a number : ";
	cin>>num;
	cout<<"\nCache Unfriendly Sieve : \nnumber of primes : "<<CacheUnfriendlySieve(num)<<"\nExecution Time : "<<t<<endl;
	cout<<"\nCache Friendly Sieve : \nnumber of primes : "<<CacheFriendlySieve(num)<<"\nExecution Time : "<<t<<endl;
	cout<<"\nCache Parallel Sieve : \nnumber of primes : "<<CacheParallelSieve(num)<<"\nExecution Time : "<<t<<endl;
	return 0;
}
