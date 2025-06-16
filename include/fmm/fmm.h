#ifndef _RS_FMM_H_
#define _RS_FMM_H_

#include <RS.h>
#include <complex>

typedef int (*FMMGetEpsilon)(const RS_Simulation *S, const RS_Layer *L, int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

int FMMGetEpsilon_Kottke(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_ClosedForm(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_FFT(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_Experimental(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

// The following need to be called after FFT or ClosedForm
int FMMGetEpsilon_PolBasisNV(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_PolBasisVL(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);
int FMMGetEpsilon_PolBasisJones(const RS_Simulation *S, const RS_Layer *L, const int n, std::complex<double> *Epsilon2, std::complex<double> *Epsilon_inv);

double GetLanczosSmoothingOrder(const RS_Simulation *S);
double GetLanczosSmoothingFactor(double order, int power, double f[2]);

#endif // _RS_FMM_H_
