#pragma once

#include <stdio.h>

// Note: the interface described in this file must be kept C-compatible.

#ifdef __cplusplus
#include <complex>
extern "C" {
#endif
#include "pattern/pattern.h"
#ifdef __cplusplus
}
#endif


#ifdef ENABLE_RS_TRACE
# ifdef __cplusplus
#  include <cstdio>
# else
#  include <stdio.h>
# endif
# define RS_TRACE(...) fprintf(stderr, __VA_ARGS__)
# define RS_CHECK if(1)
#else
# define RS_TRACE(...)
# define RS_CHECK if(0)
#endif




#define RS_VERB(verb,...) \
	do{ \
		if(S->options.verbosity >= verb){ \
			fprintf(stdout, "[%d] ", verb); \
			fprintf(stdout, __VA_ARGS__); \
		} \
	}while(0)

#ifdef __cplusplus
# include <cstdlib>
#else
# include <stdlib.h>
#endif

void* RS_malloc(size_t size);
void RS_free(void *ptr);

typedef struct{
	char *name;    // name of material
	int type; // 0 = scalar epsilon, 1 = tensor
	union{
		double s[2]; // real and imaginary parts of epsilon
		double abcde[10];
		// [ a b 0 ]
		// [ c d 0 ]
		// [ 0 0 e ]
	} eps;
} RS_Material;

struct LayerModes;
typedef struct{
	char *name;       // name of layer
	double thickness; // thickness of layer
	RS_MaterialID material;   // name of background material
	Pattern pattern;  // See pattern.h
	RS_LayerID copy;       // See below.
	struct LayerModes *modes;
} RS_Layer;
// If a layer is a copy, then `copy' is the name of the layer that should
// be copied, and `material' and `pattern' are inherited, and so they can
// be arbitrary. For non-copy layers, copy should be NULL.

struct FieldCache;

typedef struct Excitation_Planewave_{
	double hx[2],hy[2]; // re,im components of H_x,H_y field
	size_t order;
	int backwards;
} Excitation_Planewave;

typedef struct Excitation_Dipole_{
	double pos[2];
	double moment[6]; // dipole moment (jxr, jxi, jyr, jyi, jzr, jzi)
	// The source is at the layer interface after the layer specified by ex_layer.
} Excitation_Dipole;

typedef struct Excitation_Exterior_{
	size_t n;
	int *Gindex1; /* 1-based G index, negative means backwards (length n) */
	double *coeff; /* complex coefficients of each G index (length 2n) */
} Excitation_Exterior;

typedef struct Excitation_{
	union{
		Excitation_Planewave planewave; // type 0
		Excitation_Dipole dipole; // type 1
		Excitation_Exterior exterior; // type 2
	} sub;
	int type;
	RS_Layer *layer; // name of layer after which excitation is applied
} Excitation;

struct Solution_;
struct RS_Simulation_{
	double Lr[4]; // real space lattice:
	              //  {Lr[0],Lr[1]} is the first basis vector's x and y coords.
	              //  {Lr[2],Lr[3]} is the second basis vector's x and y coords.
	double Lk[4]; // reciprocal lattice:
	              //  2pi*{Lk[0],Lk[1]} is the first basis vector's x and y coords.
	              //  2pi*{Lk[2],Lk[3]} is the second basis vector's x and y coords.
	              // Computed by taking the inverse of Lr as a 2x2 column-major matrix.
	int n_G;            // Number of G-vectors (Fourier planewave orders).
	int *G; // length 2*n_G; uv coordinates of Lk
	double *kx, *ky; // each length n_G
	int n_materials, n_materials_alloc;
	RS_Material *material; // array of materials
	int n_layers, n_layers_alloc;
	RS_Layer *layer;       // array of layers

	// Excitation
	double omega[2]; // real and imaginary parts of omega
	double k[2]; // xy components of k vector, as fraction of k0 = omega
	Excitation exc;

	struct Solution_ *solution; // The solution object is not allocated until needed.
	RS_Options options;

	struct FieldCache *field_cache; // Internal cache of vector field FT when using polarization bases
	
	RS_message_handler msg;
	void *msgdata;
};

#ifdef __cplusplus
extern "C" {
#endif



//// Layer methods
void Layer_Init(RS_Layer *L, const char *name, double thickness, const char *material, const char *copy);
void Layer_Destroy(RS_Layer *L);

//// Material methods
void Material_Init(RS_Material *M, const char *name, const double eps[2]);
void Material_InitTensor(RS_Material *M, const char *name, const double abcde[10]);
void Material_Destroy(RS_Material *M);

//// Simulation methods
/*
void Simulation_Init(RS_Simulation *S, const double *Lr, unsigned int nG, int *G);
void Simulation_Destroy(RS_Simulation *S);
void Simulation_Clone(const RS_Simulation *S, RS_Simulation *T);
*/
void Simulation_DestroySolution(RS_Simulation *S);
void Simulation_DestroyLayerSolutions(RS_Simulation *S);
void Simulation_DestroyLayerModes(RS_Layer *layer);
void RS_Simulation_DestroyLayerModes(RS_Simulation *S, RS_LayerID id);

// Destroys the solution belonging to a given simulation and sets
// S->solution to NULL.

///////////////// Simulation parameter specifications /////////////////
// These will invalidate any existing partially computed solutions.

// Assumes that S->Lr is filled in, and computes Lk.
// For 1D lattices, S->Lr[2] = S->Lr[3] = 0, and the corresponding
// Lk[2] and Lk[3] are set to 0.
// Returns:
//  -1 if S is NULL
//   0 on success
//   1 if basis vectors are degenerate (rank Lr < 2)
//   2 if basis vectors are zero (Lr == 0)
int Simulation_MakeReciprocalLattice(RS_Simulation *S);

RS_Material* Simulation_AddMaterial(RS_Simulation *S);
// Adds a blank material to the end of the array in S and returns
// a pointer to it.

RS_Layer* Simulation_AddLayer(RS_Simulation *S);
// Adds a blank layer to the end of the array in S and returns
// a pointer to it.

int Simulation_SetNumG(RS_Simulation *S, int n);
int Simulation_GetNumG(const RS_Simulation *S, int **G);
double Simulation_GetUnitCellSize(const RS_Simulation *S);

// Returns NULL if the layer name could not be found, or on error.
// If index is non-NULL, the offset of the layer in the list is returned.
RS_Layer* Simulation_GetLayerByName(const RS_Simulation *S, const char *name, int *index);

// Returns NULL if the material name could not be found, or on error.
// If index is non-NULL, the offset of the material in the list is returned.
RS_Material* Simulation_GetMaterialByName(const RS_Simulation *S, const char *name, int *index);
RS_Material* Simulation_GetMaterialByIndex(const RS_Simulation *S, int i);

// Returns -n if the n-th argument is invalid.
// S and L should be valid pointers, material is the offset into the material list (not bounds checked).
// angle should be in radians
// vert should be of length 2*nvert
int Simulation_AddLayerPatternCircle   (RS_Simulation *S, RS_Layer *layer, int material, const double center[2], double radius);
int Simulation_AddLayerPatternEllipse  (RS_Simulation *S, RS_Layer *layer, int material, const double center[2], double angle, const double halfwidths[2]);
int Simulation_AddLayerPatternRectangle(RS_Simulation *S, RS_Layer *layer, int material, const double center[2], double angle, const double halfwidths[2]);
int Simulation_AddLayerPatternPolygon  (RS_Simulation *S, RS_Layer *layer, int material, const double center[2], double angle, int nvert, const double *vert);
int Simulation_RemoveLayerPatterns(RS_Simulation *S, RS_Layer *layer);
int Simulation_ChangeLayerThickness(RS_Simulation *S, RS_Layer *layer, const double *thickness);

// Returns 14 if no layers present
// exg is length 2*n, pairs of G index (1-based index, negative for backwards modes), and 0,1 polarization (E field x,y)
// ex is length 2*n, pairs of re,im complex coefficients
int Simulation_MakeExcitationExterior(RS_Simulation *S, int n, const int *exg, const double *ex);
int Simulation_MakeExcitationPlanewave(RS_Simulation *S, const double angle[2], const double pol_s[2], const double pol_p[2], size_t order);
int Simulation_MakeExcitationDipole(RS_Simulation *S, const double k[2], const char *layer, const double pos[2], const double moment[6]);


// Internal functions
#ifdef __cplusplus
// Field cache manipulation
void Simulation_InvalidateFieldCache(RS_Simulation *S);
std::complex<double>* Simulation_GetCachedField(const RS_Simulation *S, const RS_Layer *layer);
void Simulation_AddFieldToCache(RS_Simulation *S, const RS_Layer *layer, size_t n, const std::complex<double> *P, size_t Plen);
#endif

//////////////////////// Simulation solutions ////////////////////////
// These functions try to compute the minimum amount necessary to
// obtain the desired result. All partial solutions are stored so
// that later requerying of the same results returns the stored
// result.

// Solution error codes:
// -n - n-th argument is invalid
// 0  - successful
// 1  - allocation error
// 9  - lattice n_G not set
// 10 - copy referenced an unknown layer name
// 11 - a copy of a copy was made
// 12 - duplicate layer name found
// 13 - excitation layer name not found
// 14 - no layers
// 15 - material not found


// Sets up S->solution; if one already exists, destroys it and allocates a new one
// Possibly reduces S->n_G
// Returns a solution error code
int Simulation_InitSolution(RS_Simulation *S);

// Returns a solution error code
int Simulation_SolveLayer(RS_Simulation *S, RS_Layer *layer);

// Returns a solution error code
// powers[0] - 0.5 real forw
// powers[1] - 0.5 real back
// powers[2] - imag forw
// powers[3] - imag back
int Simulation_GetPoyntingFlux(RS_Simulation *S, RS_Layer *layer, double offset, double powers[4]);
int Simulation_GetPoyntingFluxByG(RS_Simulation *S, RS_Layer *layer, double offset, double *powers);

// Returns a list of S->n_G complex numbers of mode propagation constants
// q should be length 2*S->n_G
int Simulation_GetPropagationConstants(RS_Simulation *S, RS_Layer *layer, double *q);

// Returns lists of 2*S->n_G complex numbers of forward and backward amplitudes
// forw and back should each be length 4*S->n_G
int Simulation_GetAmplitudes(RS_Simulation *S, RS_Layer *layer, double offset, double *forw, double *back);
// waves should be size 2*11*S->n_G
// Each wave is:
//   { kx, ky, kzr, kzi, ux, uy, uz, cur, cui, cvr, cvi }
int Simulation_GetWaves(RS_Simulation *S, RS_Layer *layer, double *wave);

// Returns a solution error code
// Tint is a vector of time averaged stress tensor integral
int Simulation_GetStressTensorIntegral(RS_Simulation *S, RS_Layer *layer, double offset, double Tint[6]);

// Returns a solution error code
// which can be 'U', 'E', 'H', 'e'
// 'E' is epsilon*|E|^2, 'H' is |H|^2, 'e' is |E|^2, 'U' is 'E'+'H'
int Simulation_GetLayerVolumeIntegral(RS_Simulation *S, RS_Layer *layer, char which, double integral[2]);
int Simulation_GetLayerZIntegral(RS_Simulation *S, RS_Layer *layer, const double r[2], double integral[6]);

// Outputs a POV-Ray render script of a unit cell of the structure.
// Return value can be ignored for valid inputs.
int Simulation_OutputStructurePOVRay(RS_Simulation *S, FILE *fp);

// Outputs a PostScript rendering of the layer pattern to stdout.
// Return value can be ignored for valid inputs.
int Simulation_OutputLayerPatternDescription(RS_Simulation *S, RS_Layer *layer, FILE *fp);

// Returns a solution error code
// Outputs the Fourier reconstruction of the layer pattern to stdout in Gnuplot splot format.
// The unit cell is discretized into nu and nv cells in the lattice directions.
int Simulation_OutputLayerPatternRealization(RS_Simulation *S, RS_Layer *layer, int nu, int nv, FILE *fp);

// Returns a solution error code
// E field is stored as {Exr,Eyr,Ezr,Exi,Eyi,Ezi}
int Simulation_GetField(RS_Simulation *S, const double r[3], double fE[6], double fH[6]);
int Simulation_GetEField(RS_Simulation *S, const double r[3], std::complex<double> efield[3], int solvetype = 0);
int Simulation_GetFieldPlane(RS_Simulation *S, int nxy[2], double z, double *E, double *H);
int Simulation_GetEFieldPlane(RS_Simulation *S, int nxy[2], double z, double *E, int solvetype = 0);

// Returns a solution error code
int Simulation_GetEpsilon(RS_Simulation *S, const double r[3], double eps[2]); // eps is {real,imag}

// Returns a solution error code
// Determinant is (rmant[0]+i*rmant[1])*base^expo
int Simulation_GetSMatrixDeterminant(RS_Simulation *S, double rmant[2], double *base, int *expo);


#ifdef __cplusplus
}
#endif
