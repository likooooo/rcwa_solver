#include "fft_iface.h"
#include <cstdlib>

#include <fftw3.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
static pthread_mutex_t mutex;
#endif

int fft_next_fast_size(int n){
    while(1){
        int m=n;
        while( (m%2) == 0 ) m/=2;
        while( (m%3) == 0 ) m/=3;
        while( (m%5) == 0 ) m/=5;
        if(m<=1)
            break; /* n is completely factorable by twos, threes, and fives */
        n++;
    }
    return n;
}

std::complex<double> *fft_alloc_complex(size_t n){
	return (std::complex<double>*)(fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
}

void fft_free(void *p){
	fftw_free(p);
}

struct tag_fft_plan{
	fftw_plan plan;
};

fft_plan fft_plan_dft_2d(
	int n[2],
	std::complex<double> *in, std::complex<double> *out,
	int sign
){
	fft_plan plan = NULL;
# ifdef HAVE_LIBPTHREAD
	pthread_mutex_lock(&mutex);
# endif
	fftw_plan p;
	p = fftw_plan_dft(2, n, (fftw_complex*)in, (fftw_complex*)out, sign, FFTW_ESTIMATE);
# ifdef HAVE_LIBPTHREAD
	pthread_mutex_unlock(&mutex);
# endif
	if(NULL != p){
		plan = (fft_plan)malloc(sizeof(tag_fft_plan));
		plan->plan = p;
	}
	return plan;
}

void fft_plan_exec(const fft_plan plan){
	fftw_execute(plan->plan);
}

void fft_plan_destroy(fft_plan plan){
	if(NULL == plan){ return; }
# ifdef HAVE_LIBPTHREAD
	pthread_mutex_lock(&mutex);
# endif
	fftw_destroy_plan(plan->plan);
# ifdef HAVE_LIBPTHREAD
	pthread_mutex_unlock(&mutex);
# endif
	free(plan);
}

void fft_init(){
#ifdef HAVE_LIBPTHREAD
	if(pthread_mutex_init(&mutex, NULL)){
        printf("Unable to initialize a mutex for FFT module\n");
    }
#endif
}

void fft_destroy(){
	fftw_cleanup();
#ifdef HAVE_LIBPTHREAD
    pthread_mutex_destroy(&mutex);
#endif
}

