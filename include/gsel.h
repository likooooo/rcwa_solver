// Purpose:
//   Selects the G vectors for a given lattice Lk, and a truncation order NG
//   The returned number of vectors in G is always less than or equal to NG.
//
// Arguments:
//   method - (INPUT) Type of truncation to use.
//            0 = circular truncation
//            1 = parallelogramic truncation
//   NG     - (IN/OUT) On input, the upper limit of the number of G vectors
//            to return (capacity of G). On exit, the number of G vectors
//            returned in G
//   Lk     - (INPUT) Primitive lattice vectors. (Lk[0],Lk[1]) are the
//            cartesian coordinates of the first lattice vector, etc.
//   G      - (OUTPUT) - Size 2*(*NG). Stores the returned G vectors as
//            successive pairs of integer coefficients of the lattice basis.
// Returns
//   -n if the n-th argument is invalid
//   0 on success
int G_select(const int method, unsigned int *NG, const double Lk[4], int *G);
