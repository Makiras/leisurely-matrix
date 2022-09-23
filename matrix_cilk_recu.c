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

void mm_base(double *restrict C, int n_C,
			 double *restrict A, int n_A,
			 double *restrict B, int n_B,
			 int n)
{
	for (int i = 0; i < n; ++i)
		for (int k = 0; k < n; ++k)
			for (int j = 0; j < n; ++j)
				C[i * n_C + j] += A[i * n_A + k] * B[k * n_B + j];
}

void mm_dac(double *restrict C, int n_C,
			double *restrict A, int n_A,
			double *restrict B, int n_B,
			int n)
{
	assert((n & (-n)) == n);
	if (n <= THRESHOLD)
		mm_base(C, n_C, A, n_A, B, n_B, n);
	else
	{
#define X(M, r, c) (M + (r * (n_##M) + c) * (n / 2))
		cilk_spawn mm_dac(X(C, 0, 0), n_C, X(A, 0, 0), n_A, X(B, 0, 0), n_B, n / 2);
		cilk_spawn mm_dac(X(C, 0, 1), n_C, X(A, 0, 0), n_A, X(B, 0, 1), n_B, n / 2);
		cilk_spawn mm_dac(X(C, 1, 0), n_C, X(A, 1, 0), n_A, X(B, 0, 0), n_B, n / 2);
		mm_dac(X(C, 1, 1), n_C, X(A, 1, 0), n_A, X(B, 0, 1), n_B, n / 2);
		cilk_sync;
		cilk_spawn mm_dac(X(C, 0, 0), n_C, X(A, 0, 1), n_A, X(B, 1, 0), n_B, n / 2);
		cilk_spawn mm_dac(X(C, 0, 1), n_C, X(A, 0, 1), n_A, X(B, 1, 1), n_B, n / 2);
		cilk_spawn mm_dac(X(C, 1, 0), n_C, X(A, 1, 1), n_A, X(B, 1, 0), n_B, n / 2);
		mm_dac(X(C, 1, 1), n_C, X(A, 1, 1), n_A, X(B, 1, 1), n_B, n / 2);
		cilk_sync;
	}
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
	mm_dac(*C, ni, *B, ni, *A, ni, ni);
	gettimeofday(&end, NULL);
	printf("%0.6f\n", tdiff(&start, &end));
	return 0;
}
