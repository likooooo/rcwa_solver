#ifndef _S4_FMM_H_
#define _S4_FMM_H_

#include <S4.h>
#include <complex>

typedef int (*FMMGetEpsilon)(const S4_Simulation *S, const S4_Layer *L, int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

int FMMGetEpsilon_Kottke(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_ClosedForm(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_FFT(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_Experimental(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

// The following need to be called after FFT or ClosedForm
int FMMGetEpsilon_PolBasisNV(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_PolBasisVL(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_PolBasisJones(const S4_Simulation *S, const S4_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

double GetLanczosSmoothingOrder(const S4_Simulation *S);
double GetLanczosSmoothingFactor(double order, int power, double f[2]);

#endif // _S4_FMM_H_
