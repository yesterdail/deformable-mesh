#include "taucs_interface.h"
#include "SparseSolver.h"

// the array of matrices
static std::vector<SparseSolver *> matrixArray;

// the array of factors for symbolic pre-factorization
class FactorEntry {
public:
	taucs_ccs_matrix *	PAP ;
	int              *	perm, * invperm;
	void			 *	SL;
	bool				hasNumericalValues;

public:
	FactorEntry()
		: PAP(NULL)
		, SL(NULL)
		, perm(NULL)
		, invperm(NULL) 
		, hasNumericalValues(false)
	{}

	~FactorEntry() {
		Clear();
	}

	void Clear() {
		if (PAP)      { taucs_ccs_free(PAP);               PAP     = 0; }
		if (SL)       { taucs_supernodal_factor_free(SL);  SL      = 0; }
		if (perm)     { free(perm);                        perm    = 0; }
		if (invperm)  { free(invperm);                     invperm = 0; }
	}
};

static std::vector<FactorEntry> factorArray;

// has to be called in the beginning before starting to work with the library
void InitTaucsInterface(bool enableLog) {
	if (enableLog)
		taucs_logfile("stdout");
}

// initializes a new matrix; returns the matrix ID
// if isSPD is true, the matrix is going to be symmetric positive definite, and only
// upper triangle values will be taken into account when building the matrix
// (so there is no need to fill in the lower triangle, but it is possible)
int  CreateMatrix(const int numRows, const int numCols, const bool isSPD) {
	SparseSolver * newMatObjectPtr = new SparseSolver(numRows, numCols, isSPD);

	int id = 0;
	for (int id = 0; id < (int)matrixArray.size(); id++) {
		if (matrixArray[id] == NULL) {
			matrixArray[id] = newMatObjectPtr;
			return id;
		}
	}

	matrixArray.push_back(newMatObjectPtr);
	return (int)matrixArray.size() - 1;
}

// releases the matrix 
void ReleaseMatrix(const int id) {
	if (id >= (int)matrixArray.size() || id < 0)
		return;

	delete matrixArray[id];
	matrixArray[id] = NULL;
}

// has to be called in the end after finishing working with the library
void DeinitTaucsInterface() {
	// release all objects
	for (int id = 0; id < (int)matrixArray.size(); id++) {
		ReleaseMatrix(id);
	}
	for (int fId = 0; fId < (int)factorArray.size(); ++fId) {
		ReleaseFactor(fId);
	}
}

// adds a new entry to the sparse matrix A
// i and j are 0-based
// if the A^T A matrix had been factored, this will destroy the factor
void SetMatrixEntry(const int id, const int i, const int j, const taucsType v) {
	if (id >= (int)matrixArray.size() || id < 0)
		return;

	matrixArray[id]->AddIJV(i, j, v);
}

// updates the entry in A by adding the value v to the current value
// it is assumed the entry is there already; otherwise it is though of as 0
// i and j are 0-based
// if the A^T A matrix had been factored, this will destroy the factor
void AddToMatrixEntry(const int id, const int i, const int j, const taucsType v) {
	if (id >= (int)matrixArray.size() || id < 0)
		return;

	matrixArray[id]->AddToIJV(i, j, v);
}

// allows to add an anchored vertex without destroying the factor, if there was one
// i is the anchor's number (i.e. the index of the mesh vertex that is anchored is i)
// w is the weight of the anchor in the original Ax=b system. HAS TO BE POSITIVE!!
// if there was no factor for A^T A, this will simply add an anchor row to the Ax=b system
// if there was a factor, the factor will be updated, approximately at the cost of
// a single solve.
void AddAnchor(const int id, const int i, const taucsType w){
	if (id >= (int)matrixArray.size() || id < 0)
		return;

	return matrixArray[id]->AddAnchor(i, w);
}


// Will create A^T A and its factorization, as well as store A^T
// returns true on success, false otherwise
bool FactorATA(const int id) {
	if (id >= (int)matrixArray.size() || id < 0)
		return false;

	return matrixArray[id]->FactorATA();
}

// Solves the normal equations for Ax = b, namely A^T A x = A^T b
// it is assumed that enough space is allocated in x
// If a factorization of A^T A exists, it will be used, otherwise it will be created
// numRhs should be the the number of right-hand sides stored in b (and respectively,
// there should be enough space allocated in the solution vector x)
// returns true on success, false otherwise
bool SolveATA(const int id, const taucsType * b, taucsType * x, const int numRhs) {
	if (id >= (int)matrixArray.size() || id < 0)
		return false;

	return matrixArray[id]->SolveATA(b, x, numRhs);
}

// Factors the A matrix alone. Assumes that A is square. If A is symmetric, Cholesky factoring is applied; otherwise LU
// After this call, only call SolveA (not SolveATA), or FactorATA to create the normal equations factor
bool FactorA(const int id) {
	if (id >= (int)matrixArray.size() || id < 0)
		return false;

	return matrixArray[id]->FactorA();
}

// Same as SolveATA only that this solves the actual system Ax = b
// It is assumed that A is rectangular and invertible
bool SolveA(const int id, const taucsType * b, taucsType * x, const int numRhs) {
	if (id >= (int)matrixArray.size())
		return false;

	return matrixArray[id]->SolveA(b, x, numRhs);
}

// The version of SolveATA with 3 right-hand sides
// returns true on success, false otherwise
bool SolveATA3(const int id, const taucsType * bx, const taucsType * by, const taucsType * bz,
								   taucsType * x,        taucsType * y,        taucsType * z)
{
	if (id >= (int)matrixArray.size() || id < 0)
		return false;

	return matrixArray[id]->SolveATA3(bx, by, bz, x, y, z);
}

// The version of SolveATA with 2 right-hand sides
// returns true on success, false otherwise
bool SolveATA2(const int id, const taucsType * bx, const taucsType * by,
								   taucsType * x,        taucsType * y)
{
	if (id >= (int)matrixArray.size() || id < 0)
		return false;

	return matrixArray[id]->SolveATA2(bx, by, x, y);
}
// Will create *symbolic* factorization of ATA and store it aside
// returns the id of the factor stored, to be used later to factor
// other matrices with the same zero structure
int SymbolicFactorATA(const int matrixId) {
	if (matrixId >= (int)matrixArray.size() || matrixId < 0)
		return -1;

	// find a place for the new factor entry in the array
	int fId;
	for (fId = 0; fId < (int)factorArray.size(); fId++) {
		if (factorArray[fId].SL == NULL) {
			break;
		}
	}
	if (fId == (int)factorArray.size()) {
		factorArray.push_back(FactorEntry());
	}

	FactorEntry & newFactor = factorArray[fId];

	// get the ATA matrix
	taucs_ccs_matrix * ATA = matrixArray[matrixId]->GetATA_Copy();
	if (ATA == NULL)
		return -1;

	// make all non-zero entries 1
	for (int col = 0; col < ATA->n; ++col) {
		for (int p = ATA->colptr[col]; p < ATA->colptr[col+1]; ++p) {
			ATA->taucs_values[p] = 1;
		}
	}
	// create symbolic factor and permutations:
	// compute re-ordering
	taucs_ccs_order(ATA, &newFactor.perm, &newFactor.invperm, "metis");
	newFactor.PAP = taucs_ccs_permute_symmetrically(ATA, newFactor.perm, newFactor.invperm);
	if (!newFactor.PAP) { 
		return -1;
	}
	// symbolic Cholesky factorization only
	newFactor.SL = taucs_ccs_factor_llt_symbolic(newFactor.PAP);

	return fId;
}


// Will create the actual numerical factorization of the given ATA matrix
// provided the symbolic factor with id factorId
bool FactorATA_UseSymbolic(const int matrixID, const int symbFactorId) {
	if (matrixID >= (int)matrixArray.size() || symbFactorId >= (int)factorArray.size()
		 || matrixID < 0 || symbFactorId < 0)
		return false;

	if (factorArray[symbFactorId].hasNumericalValues) {
		taucs_supernodal_factor_free_numeric(factorArray[symbFactorId].SL);
	}

	factorArray[symbFactorId].hasNumericalValues = true;
	taucs_ccs_free(factorArray[symbFactorId].PAP);

	return matrixArray[matrixID]->FactorATA_UseSymbolic(factorArray[symbFactorId].SL,
														&factorArray[symbFactorId].PAP, 
														factorArray[symbFactorId].perm,
														factorArray[symbFactorId].invperm);
}

// Will solve the LS system using the precomputed factor
bool SolveATA_UseFactor(const int matrixId, const int factorId, const taucsType * b, taucsType * x, const int numRhs) {
	if (matrixId >= (int)matrixArray.size() || factorId >= (int)factorArray.size()
		|| matrixId < 0 || factorId < 0)
		return false;

	return matrixArray[matrixId]->SolveATA_UseFactor(factorArray[factorId].SL, 
													 factorArray[factorId].PAP, 
													 factorArray[factorId].perm,
													 factorArray[factorId].invperm,
													 b, x, numRhs);
}


// will free the factor
void ReleaseFactor(const int factorId) {
	if (factorId >= (int)factorArray.size() || factorId < 0)	
		return;

	factorArray[factorId].Clear();
}

// VectorAdd stores the result of v1 + v2 in result; assumes all the three vectors
// have allocated size of length elements
// result may be the same pointer as v1 or v2
void VectorAdd(const taucsType * v1, const taucsType * v2, taucsType * result, int length) {
	for (int i = 0; i < length; ++i)
		result[i] = v1[i] + v2[i];
}

// VectorSubtract stores the result of v1 - v2 in result; assumes all the three vectors
// have allocated size of length elements
// result may be the same pointer as v1 or v2
void VectorSubtract(const taucsType * v1, const taucsType * v2, taucsType * result, int length) {
	for (int i = 0; i < length; ++i)
		result[i] = v1[i] - v2[i];
}

// Stores the result of Matrix*v in result.
// v can have numCols columns; then the result is stored column-wise as well
// It is assumed space is allocaed in result!
// v cannot be the same pointer as result
bool MultiplyMatrixVector(const int idMatrix, const taucsType * v, taucsType * result, const int numCols) {
	if (idMatrix < 0 || idMatrix >= (int)matrixArray.size())
		return false;

	matrixArray[idMatrix]->MultiplyMatrixVector(v, result, numCols);
	return true;
}

// Computes D*Matrix and stores the result in Res
// D holds the diagonal values
bool MultiplyDiagMatrixMatrix(const int idMatrix, const taucsType * D, const int idRes) {
	if (idMatrix < 0 || idMatrix >= (int)matrixArray.size() || idRes < 0 || idRes >= (int)matrixArray.size())
		return false;

	matrixArray[idMatrix]->MultiplyDiagMatrixMatrix(D, *(matrixArray[idRes]));
	return true;
}

// Multiplies the A matrix by the given matrix B from the right (A*B)
// and stores the result in the columns of res
// isSPD tells us whether the result should be SPD or not
bool MultiplyMatrixMatrix(const int idA, const int idB, const int idRes, const bool isSPD) {
	if (idA		< 0	|| idA		>= (int)matrixArray.size() || 
		idB		< 0	|| idB		>= (int)matrixArray.size() ||
		idRes	< 0	|| idRes	>= (int)matrixArray.size())
		return false;

	return matrixArray[idA]->MultiplyMatrixRight(*(matrixArray[idB]),*(matrixArray[idRes]), isSPD);
}

// Transposes the given matrix
// and stores the result in idRes
bool TransposeMatrix(const int idA, const int idRes) {
	if (idA		< 0	|| idA		>= (int)matrixArray.size() ||
		idRes	< 0	|| idRes	>= (int)matrixArray.size())
		return false;

	return matrixArray[idA]->Transpose(*(matrixArray[idRes]));
}

// will remove the rows startInd-endInd (including ends, zero-based) from Matrix
// will discard any factors
bool DeleteRowRange(const int idMatrix, const int startInd, const int endInd) {
	if (idMatrix < 0 || idMatrix >= (int)matrixArray.size())
		return false;

	return matrixArray[idMatrix]->DeleteRowRange(startInd, endInd);
}

// will remove the columns startInd-endInd (including ends, zero-based) from Matrix
// will discard any factors
bool DeleteColumnRange(const int idMatrix, const int startInd, const int endInd) {
		if (idMatrix < 0 || idMatrix >= (int)matrixArray.size())
		return false;

	return matrixArray[idMatrix]->DeleteColumnRange(startInd, endInd);
}

// copies columns startInd through endInd (including, zero-based) from A to B
// previous data in B, including factors, is erased.
bool CopyColumnRange(const int idA, const int startInd, const int endInd, const int idB) {
	if (idA	< 0	|| idA >= (int)matrixArray.size() || 
		idB	< 0	|| idB >= (int)matrixArray.size())
		return false;

	return matrixArray[idA]->CopyColumnRange(startInd, endInd, *(matrixArray[idB]));
}

// copies rows startInd through endInd (including, zero-based) from A to B
// previous data in B, including factors, is erased.
bool CopyRowRange(const int idA, const int startInd, const int endInd, const int idB) {
	if (idA	< 0	|| idA >= (int)matrixArray.size() || 
		idB	< 0	|| idB >= (int)matrixArray.size())
		return false;

	return matrixArray[idA]->CopyRowRange(startInd, endInd, *(matrixArray[idB]));
}

bool MultiplyRows(const int idA, const int startInd, const int endInd, double val) {
	if (idA	< 0	|| idA >= (int)matrixArray.size())
		return false;

	return matrixArray[idA]->MultiplyRows(startInd, endInd, val);
}


// returns the dimensions of the queried matrix
bool GetAMatrixSize(const int idA, int & numRows, int & numCols) {
	if (idA < 0 || idA >= (int)matrixArray.size())
		return false;

	matrixArray[idA]->GetAMatrixSize(numRows, numCols);
	return true;
}

// explicitly states that the matrix is spd or not
bool SetSPD(const int id, const bool isSPD) {
	if (id < 0 || id >= (int)matrixArray.size())
		return false;

	matrixArray[id]->SetSPD(isSPD);
	return true;
}
