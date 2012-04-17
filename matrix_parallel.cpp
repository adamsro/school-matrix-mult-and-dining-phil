
/*
	 - by chamika deshan
http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=12491&lngWId=3
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <boost/random.hpp>

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
int opt, file_flag = 0, serial_flag = 0, verbose_flag = 0;

int main(int argc, char *argv[]) {
		pthread_t nothreads[THREADS + 1];
		void *exitstat;
		struct timeval start, stop;

		while ((opt = getopt (argc, argv, "svf:")) != -1)
				switch (opt) {
						case 's': // perform serial matrix multiplication
								serial_flag = 1;
								break;
						case 'v':
								verbose_flag = 1; 
						case 'f': // read matrix from file
								{
										file_flag = 1; 
										std::ifstream myfile;
										char *cvalue = NULL;
										cvalue = optarg;
										myfile.open (cvalue);
										std::cout << cvalue;
										//while(std::cout << myfile) {
										//;
										//}
										//int i = 0;
										//for (int p = 0; p < N; p++) {
										//for (int q = 0; q < N; q++) {
										//m[p * N + q] = val[i++];
										//}
										//}
										myfile.close();	
										break;				
								}
						case '?':
								if (optopt == 'f')
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

		// for testing
		//fillMatrix(a[0], 1.0);
		//fillMatrix(b[0], 2.0);
		//fillMatrix(c[0], 0);

		if(verbose_flag == 1) {
				printf("====Maatrix A====\n");
				printmatrix(a[0]);
				printf("====Maatrix B====\n");
				printmatrix(b[0]);
		}

		if(serial_flag == 1) {
				for (int i = 0; i < N; i++) {
						for (int j = 0; j < N; j++) {
								for (int k = 0; k < N; k++) {
										c[i][k] += a[i][j] * b[j][k];
								}
						}
				}
		} else {

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
		}
		if(verbose_flag == 1) {
				fprintf(stdout, "Time = %.6f\n\n", (stop.tv_sec + stop.tv_usec * 1e-6)-(start.tv_sec + start.tv_usec * 1e-6));
				printmatrix(*c);
		}

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
		if(verbose_flag == 1) {
				printf("id = %ld perprocess = %d startpoint = %d and endpoint = %d\n", id, endpoint - startpoint, startpoint, endpoint);
		}
		for (int i = startpoint; i < endpoint; i++) {
				for (int j = 0; j < N; j++) {
						for (int k = 0; k < N; k++) {
								c[i][k] += a[i][j] * b[j][k];
						}
				}
		}
}

void fillMatrix(float* m, float val) {
		for (int p = 0; p < N; p++) {
				for (int q = 0; q < N; q++) {
						m[p * N + q] = val;
				}
		}
}


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

