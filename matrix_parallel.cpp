//**************************************
// Name: Pthreads matrix multiplication
// Description:Parallel matrix multiplication using pthreads sample code.
// By: chamika deshan
//
//This code is copyrighted and has// limited warranties.Please see http://www.Planet-Source-Code.com/vb/scripts/ShowCode.asp?txtCodeId=12491&lngWId=3//for details.//**************************************

/*
	 - This is a parallel matrix multiplication(supports only for squre matrices) using pthreads
	 - this code have been tested on linux gcc and windows cygwin
	 - by chamika deshan
	 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <boost/random.hpp>
//#include <boost/random/uniform_real.hpp>
//#include <boost/random/variate_generator.hpp>
//#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

#define THREADS 4

#if !defined(N)
#define N 10 
#endif

void fillMatrixRand(float* m);
void fillMatrix(float* ptr, float val);
void printmatrix(float* ptr);
void *doCalculate(void *ptr);
float a[N][N], b[N][N], c[N][N];
typedef boost::minstd_rand base_generator_type;

float getRandom(int min, int max) {
  //timeval t;
  //gettimeofday(&t,NULL);
  //boost::mt19937 seed((int)t.tv_sec);
  //boost::uniform_real<float> dist(min, max);
  //boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > myrand(seed, dist);
	 //boost::uniform_real<> uni_dist(0,1);
// Define a random number generator and initialize it with a reproducible
  // seed.
	
}

int main(int argc, char *argv[]) {
		int opt, file_flag = 0; //, serial_flag = 0;
		pthread_t nothreads[THREADS + 1];
		void *exitstat;
		struct timeval start, stop;
		//float seed1[N];
		//float seed2[N];

		while ((opt = getopt (argc, argv, "fs:")) != -1)
				switch (opt) {
						case 'f': // read matrix from file
								{
										file_flag = 1; 
										std::ifstream myfile;
										myfile.open (optarg);
										while(std::cout << myfile) {
												;
										}
		//int i = 0;
		//for (int p = 0; p < N; p++) {
				//for (int q = 0; q < N; q++) {
						//m[p * N + q] = val[i++];
				//}
		//}
										myfile.close();	
										break;				
								}
						case 's': // perform serial matrix multiplication
								for (int i = 0; i < N; i++) {
										for (int j = 0; j < N; j++) {
												for (int k = 0; k < N; k++) {
														c[i][k] += a[i][j] * b[j][k];
												}
										}
								}
								printmatrix(*c);
								return 0;
								break;
						case '?':
								if (optopt == 'c')
										fprintf (stderr, "Option -%c requires an argument.\n", optopt);
								else if (isprint (optopt))
										fprintf (stderr, "Unknown option `-%c'.\n", optopt);
								else
										fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
								return 1;
								//default:
				}
		if(file_flag == 0) {
				fillMatrixRand(a[0]);
				fillMatrixRand(b[0]);
				fillMatrix(c[0], 0);
		}

		//fillMatrix(a[0], 1.0);
		//fillMatrix(b[0], 2.0);
		//fillMatrix(c[0], 0);

		//Uncomment to print matrix A and B
		printf("====Maatrix A====\n");
		printmatrix(a[0]);
		printf("====Maatrix B====\n");
		printmatrix(b[0]);

		gettimeofday(&start, 0);
		//-- thread creation goes here
		for (long i = 1; i <= THREADS; i++) {
				if (pthread_create(&nothreads[i], NULL, doCalculate, (void *) i) != 0)
						perror("Thread creation failed");
		}
		//-- thread joining goes here
		for (long i = 1; i <= THREADS; i++) {
				if (pthread_join(nothreads[i], &exitstat) != 0)
						perror("joining failed");
		}
		gettimeofday(&stop, 0);
		fprintf(stdout, "Time = %.6f\n\n", (stop.tv_sec + stop.tv_usec * 1e-6)-(start.tv_sec + start.tv_usec * 1e-6));
		/* uncomment to print matrix c
		 * printmatrix(c[0]);
		 */
		printmatrix(*c);
		exit(0);
}

//---- this is the function executed by each thread
void *doCalculate(void *ptr) {
		int rowsperprocess = N / THREADS;
		long id = (long) ptr;
		int startpoint, endpoint;
		startpoint = (id - 1) * rowsperprocess;
		if (id == THREADS) {
				endpoint = startpoint + rowsperprocess + (N % THREADS);
		} else {
				endpoint = startpoint + rowsperprocess;
		}
		printf("id = %ld perprocess = %d startpoint = %d and endpoint = %d\n", id, endpoint - startpoint, startpoint, endpoint);
		for (int i = startpoint; i < endpoint; i++) {
				for (int j = 0; j < N; j++) {
						for (int k = 0; k < N; k++) {
								c[i][k] += a[i][j] * b[j][k];
						}
				}
		}
}

/*
 * val must have enough values to fill the matrix
 */
void fillMatrix(float* m, float val) {
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						m[p * N + q] = val;
				}
		}
}

/*
 * val must have enough values to fill the matrix
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
						//m[p * N + q] = getRandom(-10.f, 10.f);
				}
		}
}
/*
 * Print Matrix to console
 */
void printmatrix(float* m) {
		int p, q;
		for (p = 0; p < N; p++) {
				for (q = 0; q < N; q++) {
						printf("%6.2f\t", m[p * N + q]);
				}
				printf("\n");
		}
}

