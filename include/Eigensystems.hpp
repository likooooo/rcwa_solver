#pragma once
#include "Types.hpp"

namespace rcwa_solver{

// These routines compute `cols` eigenpairs of the matrix A,
// returning the eigenvalues in vals, and the eigenvectors
// in vecs.

int Eigensystem(
	SpMat::Index cols, const SpMat &A,
	CVec &vals, CMat &vecs
);

int HermitianEigensystem(
	SpMat::Index cols, const SpMat &A,
	CVec &vals, CMat &vecs
);

} // namespace rcwa_solver
