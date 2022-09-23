#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef n
#define n 1024
#endif

double A[n][n];
double B[n][n];
double C[n][n];

#ifndef _LOOP_1_
#define _LOOP_1_ i
#endif

#ifndef _LOOP_2_
#define _LOOP_2_ j
#endif

#ifndef _LOOP_3_
#define _LOOP_3_ k
#endif

float tdiff(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}

int main(int argc, const char *argv[])
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			A[i][j] = (double)rand() / (double)RAND_MAX;
			B[i][j] = (double)rand() / (double)RAND_MAX;
			C[i][j] = 0;
		}
	}
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for (int _LOOP_1_ = 0; _LOOP_1_ < n; ++_LOOP_1_)
	{
		for (int _LOOP_2_ = 0; _LOOP_2_ < n; ++_LOOP_2_)
		{
			for (int _LOOP_3_ = 0; _LOOP_3_ < n; ++_LOOP_3_)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	gettimeofday(&end, NULL);
	printf("%0.6f\n", tdiff(&start, &end));
	return 0;
}
