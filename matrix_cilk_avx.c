#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <sys/time.h>
#include <assert.h>
#include <immintrin.h> // SSE/AVX/AVX2/AVX-512 instructions accelerate

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
	int i, j, k;
	__m512d c0, c1, c2, c3;
	for (i = 0; i < n; ++i)
		for (k = 0; k < n; k+=32)
		{
			c0 = _mm512_load_pd(&C[i * n_C + 0]);
			c1 = _mm512_load_pd(&C[i * n_C + 8]);
			c2 = _mm512_load_pd(&C[i * n_C + 16]);
			c3 = _mm512_load_pd(&C[i * n_C + 24]);
			for (j = 0; j < n; j+=32)
			{
				c0 = _mm512_fmadd_pd(_mm512_load_pd(&A[i * n_A + j]), _mm512_load_pd(&B[j * n_B + k]), c0);
				c1 = _mm512_fmadd_pd(_mm512_load_pd(&A[i * n_A + j]), _mm512_load_pd(&B[j * n_B + k + 8]), c1);
				c2 = _mm512_fmadd_pd(_mm512_load_pd(&A[i * n_A + j]), _mm512_load_pd(&B[j * n_B + k + 16]), c2);
				c3 = _mm512_fmadd_pd(_mm512_load_pd(&A[i * n_A + j]), _mm512_load_pd(&B[j * n_B + k + 24]), c3);
			}
			_mm512_store_pd(&C[i * n_C + k], c0);
			_mm512_store_pd(&C[i * n_C + k + 8], c1);
			_mm512_store_pd(&C[i * n_C + k + 16], c2);
			_mm512_store_pd(&C[i * n_C + k + 24], c3);
		}
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
