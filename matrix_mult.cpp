/*
 * Original Author:  chamika deshan
 * File: matrix_mult.c
 * Last Modified: 2012 April 17, 13:48 by Robert Adams (adamsro) 
 * 
 * This file will perform threaded or serial matrix multiplication
 *  -f filename			read matrix input from a file
 *  -v							print additional information
 *  -s							do the multiplication serialy.
 * 
 * Adapted from code written by chamika deshan found at:
 * 
 * http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=12491&lngWId=3
 */	

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <time.h>

#include <boost/random.hpp>
#include <boost/thread.hpp>

#define THREADS 4

#if !defined(N)
#define N 10 
#endif

void fillMatrixRand(float* m);
void fillMatrix(float* ptr, float val);
void fillMatrixFile(float* a, float*b, char*optarg);
void printmatrix(float* ptr);
void doCalculate(int id);

float a[N][N], b[N][N], c[N][N];
typedef boost::minstd_rand base_generator_type;
int opt, file_flag, serial_flag,  verbose_flag;

int main(int argc, char *argv[]) {
		file_flag = 0; 
		serial_flag = 0;
		verbose_flag = 0;
    time_t start;
    time_t theend;
    long ncpus;
    double total_time;

		while ((opt = getopt (argc, argv, "svf:")) != -1)
				switch (opt) {
						case 's': // perform serial matrix multiplication
								serial_flag = 1;
								break;
						case 'v':
								verbose_flag = 1; 
								break;
						case 'f': // read matrix from file
								file_flag = 1; 
								break;	
						case '?':
								if (optopt == 'f')
										fprintf (stderr, "Option -%c requires an argument.\n", optopt);
								else if (isprint (optopt))
										fprintf (stderr, "Unknown option `-%c'.\n", optopt);
								else
										fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
								return 1;
				}

		if(file_flag == 1) {
				fillMatrixFile(*a, *b, optarg);
		} else {
				fillMatrixRand(a[0]);
				fillMatrixRand(b[0]);
				fillMatrix(c[0], 0);
		}

		// for testing
		//fillMatrix(a[0], 1.0);
		//fillMatrix(b[0], 2.0);
		//fillMatrix(c[0], 0);

		if(verbose_flag == 1) {
				printf("==== Maatrix A ====\n");
				printmatrix(a[0]);
				printf("==== Maatrix B ====\n");
				printmatrix(b[0]);
		}

		start = clock();
		if(serial_flag == 1) { // no threads, just pound it out.
				for (int i = 0; i < N; i++) {
						for (int j = 0; j < N; j++) {
								for (int k = 0; k < N; k++) {
										c[i][k] += a[i][j] * b[j][k];
								}
						}
				}
		} else {
				boost::thread_group g;
				for(int i = 1; i <= THREADS; ++i) {
						boost::thread *t = new boost::thread(&doCalculate, i);
						g.add_thread(t);
				}
				g.join_all();
		}
		theend = clock();

		printmatrix(*c);

		ncpus = sysconf(_SC_NPROCESSORS_ONLN);
		total_time = (((double) (theend - start)) / (double) CLOCKS_PER_SEC);
		printf("%d\t%.6f\n", N, total_time / (THREADS < ncpus ? THREADS : ncpus));
		
		exit(0);
}

/*
 * this is the function executed by each thread
 * columns are split into N sections. one for each thread
 */
void doCalculate(int id) {
		int rowsperprocess = N / THREADS;
		int startpoint, endpoint;
		startpoint = (id - 1) * rowsperprocess;
		if (id == THREADS) {
				endpoint = startpoint + rowsperprocess + (N % THREADS);
		} else {
				endpoint = startpoint + rowsperprocess;
		}
		if(verbose_flag == 1) {
				printf("id = %d perprocess = %d startpoint = %d and endpoint = %d\n", id, endpoint - startpoint, startpoint, endpoint);
		}
		for (int i = startpoint; i < endpoint; i++) {
				for (int j = 0; j < N; j++) {
						for (int k = 0; k < N; k++) {
								c[i][k] += a[i][j] * b[j][k];
						}
				}
		}
}

/*
 * Set all values in a Matrix to a single value
 */
void fillMatrix(float* m, float val) {
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						m[p * N + q] = val;
				}
		}
}

/*
 * Fill a matrix with random values.
 */
void fillMatrixRand(float* m) {
		timeval t;
		gettimeofday(&t,NULL);
		base_generator_type generator(t.tv_usec);
		boost::uniform_real<float> uni_dist(-10.f,10.f);
		boost::variate_generator<base_generator_type&, boost::uniform_real<float> > uni(generator, uni_dist);
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						m[p * N + q] = uni();
				}
		}
}

/* 
 * Read each word in a file, try to convert it to a float, 
 * then set it to the next free element in matrix a & b
 *
 * If eof is reached, fill remaining spaces with zeros.
 */
void fillMatrixFile(float* a, float*b, char*optarg) {
		std::string word;
		std::ifstream myfile;

		std::cout << optarg;
		myfile.open(optarg);

		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						if(myfile >> word) { 
								b[p * N + q] = atof(word.c_str());
						}
						b[p * N + q] = 0;
				}
		}
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						if(myfile >> word) {
								b[p * N + q] = atof(word.c_str());
						}
						b[p * N + q] = 0;
				}
		}
		myfile.close();	
}

/*
 * Print Matrix to console
 */
void printmatrix(float* m) {
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						printf("%6.2f\t", m[p * N + q]);
				}
				printf("\n");
		}
}


