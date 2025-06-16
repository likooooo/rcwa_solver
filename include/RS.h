#pragma once

/************************* Fundamental types *************************/
typedef double RS_real;

#ifdef __cplusplus
#include <complex>
typedef std::complex<RS_real> RS_complex;
#endif

/***************************** RS types ******************************/
typedef int RS_MaterialID;
typedef int RS_LayerID;
typedef struct RS_Simulation_ RS_Simulation;

typedef struct RS_Options_{
	int use_discretized_epsilon;

	// Set use_subpixel_smoothing to nonzero if subpixel smoothing should
	// be done on the Fourier transform of the dielectric functions.
	// This will cause the dielectric function to be discretized instead
	// of using the analytic Fourier transforms of specified shapes.
	int use_subpixel_smoothing;

	// Set use_Lanczos_smoothing to nonzero if Lanczos smoothing should
	// be done on the Fourier transform of the dielectric functions.
	int use_Lanczos_smoothing;

	// Set use_polarization_basis to nonzero if a decomposition into the
	// structure conforming polarization basis should be performed. By
	// default, the basis is not everywhere-unit-normalized and purely
	// real unless one of the following two options are used.
	int use_polarization_basis;

	// Set use_jones_vector_basis to nonzero if a complex Jones
	// vector polarization basis should be used. This setting has no
	// effect if use_polarization_basis is zero.
	int use_jones_vector_basis;

	// Set use_normal_vector_basis to nonzero if a normalized normal
	// vector polarization basis should be used. This setting has no
	// effect if use_polarization_basis is zero.
	int use_normal_vector_basis;

	// Set use_normal_vector_field to nonzero if a normal vector field
	// should be generated. The field is everywhere normal to material
	// interfaces and forced to have unit magnitude everywhere.
	int use_normal_vector_field;

	// Set resolution to be the multiple of the reciprocal lattice order
	// extent by which the polarization basis vector field should be
	// discretized. For example, if the reciprocal lattice orders go up
	// to G = {2,1}, then the vector field will be generated on a grid
	// of dimension 2*f x 1*f, where f is this value.
	// It is preferable to set this value to have small integer factors
	// in order to make the FFT more efficient. Powers of 2 are best.
	// The default value is 64, and must be at least 2 to satisfy the
	// Nyquist sampling criterion.
	int resolution;

	// Set vector_field_dump_filename_prefix to non-NULL if the
	// automatically generated vector field in the polarization
	// decomposition should be output to files whose prefix is the
	// specified string (suffix is layer name).
	char *vector_field_dump_filename_prefix;

	// Specifies how G vectors should be chosen (currently not used)
	// 0 = Circular truncation
	// 1 = Parallelogramic truncation
	int lattice_truncation;

	// Specifies how much simulation output progress to output to stdout
	int verbosity;

	// Set use_experimental_fmm to nonzero if the experimental FMM
	// formulation should be used. (For experimental/debug purposes)
	int use_experimental_fmm;

	// Set use_less_memory to nonzero if intermediate matrices should not
	// be stored in memory when possible.
	int use_less_memory;

	RS_real lanczos_smoothing_width;
	int lanczos_smoothing_power;
} RS_Options;

#define RS_MSG_ERROR    1
#define RS_MSG_WARNING  3
#define RS_MSG_INFO     5
#define RS_MSG_STATUS   9
typedef int (*RS_message_handler)(
	void *data, const char *fname, int level, const char *msg
);

/**************************** Public API *****************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* Simulation object constructor, destructor, and copy constructor. */
/********************************************************************/
RS_Simulation* RS_Simulation_New(const RS_real *Lr, unsigned int nG, int *G);
void RS_Simulation_Destroy(RS_Simulation *S);
RS_Simulation* RS_Simulation_Clone(const RS_Simulation *S);

RS_message_handler RS_Simulation_SetMessageHandler(
	RS_Simulation *S, RS_message_handler handler, void *data
);

unsigned int RS_Lattice_Count(const RS_real *Lr, unsigned int nG);
int RS_Lattice_Reciprocate(const RS_real *Lr, RS_real *Lk);

/**********************************/
/* Simulation getters and setters */
/**********************************/
int RS_Simulation_SetLattice(RS_Simulation *S, const RS_real *Lr);
int RS_Simulation_GetLattice(const RS_Simulation *S, RS_real *Lr);
int RS_Simulation_SetBases(RS_Simulation *S, unsigned int nG, int *G);
int RS_Simulation_GetBases(const RS_Simulation *S, int *G);
int RS_Simulation_SetFrequency(RS_Simulation *S, const RS_real *freq_complex);
int RS_Simulation_GetFrequency(const RS_Simulation *S, RS_real *freq_complex);
int RS_Simulation_LayerCount(const RS_Simulation *S);
int RS_Simulation_TotalThickness(const RS_Simulation *S, RS_real *thickness);

/******************************/
/* Material related functions */
/******************************/
#define RS_MATERIAL_TYPE_SCALAR_REAL      2
#define RS_MATERIAL_TYPE_SCALAR_COMPLEX   3
#define RS_MATERIAL_TYPE_XYTENSOR_REAL    4
#define RS_MATERIAL_TYPE_XYTENSOR_COMPLEX 5
RS_MaterialID RS_Simulation_SetMaterial(
	RS_Simulation *S, RS_MaterialID M, const char *name, int type, const RS_real *eps
);
RS_MaterialID RS_Simulation_GetMaterialByName(
	const RS_Simulation *S, const char *name
);
int RS_Material_GetName(
	const RS_Simulation *S, RS_MaterialID M, const char **name
);
int RS_Material_GetEpsilon(
	const RS_Simulation *S, RS_MaterialID M, RS_real *eps
);

/***************************/
/* Layer related functions */
/***************************/
RS_LayerID RS_Simulation_SetLayer(
	RS_Simulation *S, RS_LayerID L, const char *name, const RS_real *thickness,
	RS_LayerID copy, RS_MaterialID material
); /*
if NULL == L:
	Adds a new layer with optional name and thickness
if NULL != L:
	if NULL == name, then the name is not changed.
*/
RS_LayerID RS_Simulation_GetLayerByName(
	const RS_Simulation *S, const char *name
);
int RS_Layer_GetName(
	const RS_Simulation *S, RS_LayerID L, const char **name
);
int RS_Layer_GetThickness(
	const RS_Simulation *S, RS_LayerID L, RS_real *thickness
);


/**********************************/
/* Layer region related functions */
/**********************************/
int RS_Layer_ClearRegions(
	RS_Simulation *S, RS_LayerID L
);
#define RS_REGION_TYPE_INTERVAL   11
#define RS_REGION_TYPE_RECTANGLE  12
#define RS_REGION_TYPE_ELLIPSE    13
#define RS_REGION_TYPE_CIRCLE     14
int RS_Layer_SetRegionHalfwidths(
	RS_Simulation *S, RS_LayerID L, RS_MaterialID M,
	int type, const RS_real *halfwidths,
	const RS_real *center, const RS_real *angle_frac
);

#define RS_REGION_TYPE_POLYGON    21
int RS_Layer_SetRegionVertices(
	RS_Simulation *S, RS_LayerID L, RS_MaterialID M,
	int type, int nv, const RS_real *v,
	const RS_real *center, const RS_real *angle_frac
);
int RS_Layer_IsCopy(RS_Simulation *S, RS_LayerID L);

/********************************/
/* Excitation related functions */
/********************************/

int RS_Simulation_ExcitationPlanewave(
	RS_Simulation *S, const RS_real *kdir, const RS_real *udir,
	const RS_real *amp_u, const RS_real *amp_v
);
int RS_Simulation_ExcitationExterior(RS_Simulation *S, int n, const int *exg, const double *ex);
int RS_Simulation_ExcitationDipole(RS_Simulation *S, const double k[2], const char *layer, const double pos[2], const double moment[6]);

/***********************************/
/* Solution hint related functions */
/***********************************/
int RS_Simulation_SolveLayer(RS_Simulation *S, RS_LayerID L);

/****************************/
/* Output related functions */
/****************************/

int RS_Simulation_GetPowerFlux(
	RS_Simulation *S, RS_LayerID layer, const RS_real *offset,
	RS_real *power
);
int RS_Simulation_GetPowerFluxes(
	RS_Simulation *S, RS_LayerID layer, const RS_real *offset,
	RS_real *power
);
// waves should be size 2*11*S->n_G
// Each wave is length 11:
//   { kx, ky, kzr, kzi, ux, uy, uz, cur, cui, cvr, cvi }
// The first n_G waves are forward propagating, the second are backward.
// Each set of n_G waves are ordered in the basis ordering.
int RS_Simulation_GetWaves(RS_Simulation *S, RS_LayerID layer, RS_real *wave);

int RS_Simulation_GetFieldPlane(
	RS_Simulation *S, const int nxy[2], const RS_real *xyz0,
	RS_real *E, RS_real *H
);

int RS_Simulation_GetEpsilon(
	RS_Simulation *S, int nxy[2], const RS_real *xyz0, RS_real *eps
); // eps is {real,imag}

// Returns a solution error code
// Tint is a vector of time averaged stress tensor integral
int RS_Simulation_GetStressTensorIntegral(
	RS_Simulation *S, RS_LayerID layer, const RS_real *offset,
	RS_real *Tint
);

// Returns a solution error code
// which can be 'U', 'E', 'H', 'e'
// 'E' is epsilon*|E|^2, 'H' is |H|^2, 'e' is |E|^2, 'U' is 'E'+'H'
#define RS_VOLUME_INTEGRAL_ENERGY_E  'E'
#define RS_VOLUME_INTEGRAL_ENERGY_H  'H'
#define RS_VOLUME_INTEGRAL_ENERGY    'U'
#define RS_VOLUME_INTEGRAL_E_SQUARED 'e'
int RS_Simulation_GetLayerVolumeIntegral(
	RS_Simulation *S, RS_LayerID layer, int which, RS_real *integral
);
int RS_Simulation_GetLayerZIntegral(
	RS_Simulation *S, RS_LayerID layer, const RS_real *r,
	RS_real *integral
);

/***************************************/
/* Mode/band-solving related functions */
/***************************************/
// Determinant is (rmant[0]+i*rmant[1])*base^expo
int RS_Simulation_GetSMatrixDeterminant(
	RS_Simulation *S, const RS_real *k,
	RS_real *rmant, RS_real *base, int *expo
);


#ifdef __cplusplus
} /* extern "C" */
#endif

#include "RS_internal.h"
