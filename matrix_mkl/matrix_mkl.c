#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <mkl.h> // Intel MKL Lib

#ifndef NI
#define NI 2048
#endif

double A[NI][NI];
double B[NI][NI];
double C[NI][NI];

float tdiff(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}

int main(int argc, const char *argv[])
{
	for (int i = 0; i < NI; ++i)
	{
		for (int j = 0; j < NI; ++j)
		{
			A[i][j] = (double)rand() / (double)RAND_MAX;
			B[i][j] = (double)rand() / (double)RAND_MAX;
			C[i][j] = 0;
		}
	}
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	// mm_dac(*C, NI, *B, NI, *A, NI, NI);
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, NI, NI, NI, 1.0, *A, NI, *B, NI, 0.0, *C, NI);
	gettimeofday(&end, NULL);
	printf("%0.6f\n", tdiff(&start, &end));
	return 0;
}
