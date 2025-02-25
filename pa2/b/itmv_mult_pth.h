/*Global variables*/

extern int thread_count;
extern double *matrix_A;
extern double *vector_x;
extern double *vector_d;
extern double *vector_y;
extern int matrix_type;
extern int matrix_dim;
extern int no_iterations;

extern int thread_mapping;
extern int cyclic_blocksize;

#define UPPER_TRIANGULAR 1
#define BLOCK_CYCLIC 1
#define BLOCK_MAPPING 0

void parallel_itmv_mult(int);

#define THREAD_COUNT_MAX 64

#define ERROR_THRESHOLD 1e-3
