#include <config.h>

#include <cstdio>
#include <cmath>
#include <S4.h>
#include "fmm.h"

#include <limits>

extern "C" double Jinc(double x);

double GetLanczosSmoothingOrder(const S4_Simulation *S){
	double Lkuv = hypot(
		S->G[2*(S->n_G-1)+0]*S->Lk[0] + S->G[2*(S->n_G-1)+1]*S->Lk[2],
		S->G[2*(S->n_G-1)+0]*S->Lk[1] + S->G[2*(S->n_G-1)+1]*S->Lk[3]
	);
	double Lku = hypot(S->Lk[0],S->Lk[1]);
	double Lkv = hypot(S->Lk[2],S->Lk[3]);
	if(Lkv < Lku){ Lku = Lkv; } // Lku is the smaller length of the two Lk vectors
	return Lkuv + Lku;
}

/*
// non-overshooting smoothing (most of the time at least):
double GetLanczosSmoothingFactor(double mp1, int power, double f[2]){
	double fr = hypot(f[0],f[1]);
	double x = fr/mp1;
	double s = Jinc(x);
	return 0.5*(s*s + (1.-x))*s;
}
*/

double GetLanczosSmoothingFactor(double mp1, int power, double f[2]){
	double fr = hypot(f[0],f[1]);
	//fprintf(stderr, "%f\t%f\t%f\t%f\t%f\n", f[0], f[1], fr, mp1, 2*mp1+1-fr);
	double j = Jinc(fr/(2*mp1+1));
	return pow(j, power);
}

