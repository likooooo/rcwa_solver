#include <complex>
#include <cstddef>

typedef struct tag_fft_plan *fft_plan;

std::complex<double> *fft_alloc_complex(size_t n);
void fft_free(void *p);

fft_plan fft_plan_dft_2d(
	int n[2],
	std::complex<double> *in, std::complex<double> *out,
	int sign
);
void fft_plan_exec(const fft_plan plan);
void fft_plan_destroy(fft_plan plan);

int fft_next_fast_size(int n);

extern "C" void fft_init();
extern "C" void fft_destroy();
