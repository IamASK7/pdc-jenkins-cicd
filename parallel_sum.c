/*
 * PDC Lab - CI/CD with Jenkins on AWS EC2
 * Asjad Saleh (FA23-BCS-020)
 *
 * OpenMP program built and tested by the Jenkins pipeline.
 *  1. Sum of 1..N using a parallel for loop with reduction(+)
 *  2. Value of PI by numerical integration, also with reduction(+)
 * Both results are checked against the known answer. The program
 * prints RESULT: PASS and exits 0 if both are correct, otherwise
 * it prints RESULT: FAIL and exits 1, which fails the Jenkins stage.
 *
 * Compile: gcc -fopenmp -O2 -Wall -o parallel_sum parallel_sum.c
 * Run:     OMP_NUM_THREADS=4 ./parallel_sum
 */
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 100000000L          /* 100 million */
#define STEPS 100000000L      /* integration steps for PI */

int main(void) {
    int threads = omp_get_max_threads();
    printf("OpenMP threads: %d (CPU cores: %d)\n", threads, omp_get_num_procs());

    /* ---------- Task 1: parallel sum with reduction ---------- */
    long long sum = 0;
    double t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum)
    for (long i = 1; i <= N; i++)
        sum += i;
    double t_sum = omp_get_wtime() - t0;

    long long expected = (long long) N * (N + 1) / 2;
    int sum_ok = (sum == expected);
    printf("Sum 1..%ld = %lld (expected %lld) -> %s, %.4f s\n",
           N, sum, expected, sum_ok ? "OK" : "WRONG", t_sum);

    /* ---------- Task 2: PI by integrating 4/(1+x^2) from 0 to 1 ---------- */
    double step = 1.0 / (double) STEPS, total = 0.0;
    t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:total)
    for (long i = 0; i < STEPS; i++) {
        double x = (i + 0.5) * step;
        total += 4.0 / (1.0 + x * x);
    }
    double pi = step * total;
    double t_pi = omp_get_wtime() - t0;

    double error = fabs(pi - M_PI);
    int pi_ok = (error < 1e-8);
    printf("PI = %.12f (error %.2e) -> %s, %.4f s\n",
           pi, error, pi_ok ? "OK" : "WRONG", t_pi);

    if (sum_ok && pi_ok) {
        printf("RESULT: PASS\n");
        return 0;
    }
    printf("RESULT: FAIL\n");
    return 1;
}
