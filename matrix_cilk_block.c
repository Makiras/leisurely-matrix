#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <sys/time.h>
#include <assert.h>

#ifndef ni
#define ni 2048
#endif

#ifndef THRESHOLD
#define THRESHOLD 32
#endif

double A[ni][ni];
double B[ni][ni];
double C[ni][ni];

float tdiff(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}

int main(int argc, const char *argv[])
{
	for (int i = 0; i < ni; ++i)
	{
		for (int j = 0; j < ni; ++j)
		{
			A[i][j] = (double)rand() / (double)RAND_MAX;
			B[i][j] = (double)rand() / (double)RAND_MAX;
			C[i][j] = 0;
		}
	}
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	// Your code with the cilk_for loop here

	gettimeofday(&end, NULL);
	printf("%0.6f\n", tdiff(&start, &end));
	return 0;
}
