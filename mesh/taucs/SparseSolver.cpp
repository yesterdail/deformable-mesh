#include "SparseSolver.h"

extern "C" {
    void chol_update(void *vF, int index, double val,void**vetree);
}

// Will create A^T A and its factorization, as well as store A^T
// returns true on success, false otherwise
bool SparseSolver::FactorATA() {
	if (m_SPD)
		return FactorA();

	ClearFactorATA();
	if (m_ATA == NULL)
		CreateATA();

	// factorization
	int	rc = taucs_linsolve(m_ATA, &m_factorATA,0,NULL,NULL,SIVANfactor,SIVANopt_arg);
	if (rc != TAUCS_SUCCESS) 
		return false;

	return true;
}


// Will create ATA matrix and AT matrix.
// returns true on success, false otherwise
void SparseSolver::CreateATA() {
	if (m_A == NULL)
		CreateA();

	ClearMatricesATA();

	if (m_SPD) {
		// don't need to multiply because m_A is invertible and symmetric
		return;
	}
	else {
		m_AT = MatrixTranspose(m_A);
		// Multiply to get ATA:
		m_ATA = Mul2NonSymmMatSymmResult(m_AT, m_A);
	}
}

// Will create A matrix (in ccs format)
void SparseSolver::CreateA() {
	ClearMatricesA();

	if (m_SPD) 
		m_A = CreateTaucsMatrixFromColumns(m_colsA, m_numRows, TAUCS_DOUBLE|TAUCS_SYMMETRIC|TAUCS_LOWER);
	else
		m_A = CreateTaucsMatrixFromColumns(m_colsA, m_numRows, TAUCS_DOUBLE);
}


// Will create *symbolic* factorization of ATA and return it; returns NULL on failure
void * SparseSolver::SymbolicFactorATA() {
	if (m_SPD) {
		if (m_A == NULL)
			CreateA();

		return taucs_ccs_factor_llt_symbolic(m_A);
	}
	else {
		if (m_ATA == NULL)
			CreateATA();

		return taucs_ccs_factor_llt_symbolic(m_ATA);
	}
}
// Will create the actual numerical factorization of the present ATA matrix
// with the same non-zero pattern as created by SymbolicFactorATA()
bool SparseSolver::FactorATA_UseSymbolic(void * symbFactor,
										 taucs_ccs_matrix ** PAP,
										 int * perm,
										 int * invperm) 
{	
	if (m_SPD) {
		if (m_A == NULL)
			CreateA();

		*PAP = taucs_ccs_permute_symmetrically(m_A, perm, invperm);
	}
	else {
		if (m_ATA == NULL)
			CreateATA();

		*PAP = taucs_ccs_permute_symmetrically(m_ATA, perm, invperm);
	}

	// compute numerical factorization
	if (taucs_ccs_factor_llt_numeric(*PAP, symbFactor) != 0)	{
		return false;
	}
	else {
		return true;	
	}
}

// Will create A and its factorization. If A is spd Cholesky factor, otherwise LU
// returns true on success, false otherwise
// Assumes A is square and invertible
bool SparseSolver::FactorA() {
	ClearFactorA();

	if (m_A == NULL)
		CreateA();

	// factorization
	int	rc;
	if (m_SPD)
		rc = taucs_linsolve(m_A, &m_factorA,0,NULL,NULL,SIVANfactor,SIVANopt_arg);
	else
		rc = taucs_linsolve(m_A, &m_factorA,0,NULL,NULL,SIVANfactorLU,SIVANopt_arg);

	if (rc != TAUCS_SUCCESS) 
		return false;

	return true;
}

// Solves the normal equations for Ax = b, namely A^T A x = A^T b
// it is assumed that enough space is allocated in x
// Uses the factorization provided in factor
// numRhs should be the the number of right-hand sides stored in b (and respectively,
// there should be enough space allocated in the solution vector x)
// returns true on success, false otherwise
bool SparseSolver::SolveATA_UseFactor(void * factor, 
									  taucs_ccs_matrix * PAP,
									  int * perm,
									  int * invperm,
									  const taucsType * b, taucsType * x, const int numRhs) {
	if (factor == NULL) {
		return false;
	}

	if (! m_SPD) {
		// prepare right-hand side
		if ((int)m_ATb.size() != m_numCols*numRhs)
			m_ATb.resize(m_numCols*numRhs);

		// multiply right-hand side
		for (int i = 0; i < numRhs; ++i) {
			MulMatrixVector(m_AT, b + i*m_numRows, (taucsType *)&(m_ATb.front()) + i*m_numCols);
		}
	}

	//	 solve the system:
	std::vector<taucsType>  PB(m_numCols*numRhs), PX(m_numCols*numRhs);

	// permute rhs
	for (int c = 0; c < numRhs; ++c) {
		taucsType * curPB = &PB[0] + c*m_numCols;
		const taucsType * curB = (m_SPD)? (b + c*m_numCols) : (&(m_ATb.front()) + c*m_numCols);

		for (int i=0; i<m_numCols; ++i)
			curPB[i] = curB[perm[i]];
	}

	// solve by back-substitution
	int rc;
	for (int c = 0; c < numRhs; ++c) {
		rc = taucs_supernodal_solve_llt(factor, &PX[0] + c*m_numCols, &PB[0] + c*m_numCols); 
		if (rc != TAUCS_SUCCESS) {
			return false;
		}
	}

	// re-permute x
	for (int c = 0; c < numRhs; ++c) {
		taucsType * curX = x + c*m_numCols;
		taucsType * curPX = &PX[0] + c*m_numCols;

		for (int i=0; i<m_numCols; ++i)
			curX[i] = curPX[invperm[i]];
	}

	return true;
}


// Solves the normal equations for Ax = b, namely A^T A x = A^T b
// it is assumed that enough space is allocated in x
// If a factorization of A^T A exists, it will be used, otherwise it will be created
// numRhs should be the the number of right-hand sides stored in b (and respectively,
// there should be enough space allocated in the solution vector x)
// returns true on success, false otherwise
bool SparseSolver::SolveATA(const taucsType * b, taucsType * x, const int numRhs) {
	if (m_SPD) {
		return SolveA(b, x, numRhs);
	}
	else {
		if (m_factorATA == NULL) {
			bool rc = FactorATA();
			if (!rc)
				return false;
		}
	}

	// prepare right-hand side
	if ((int)m_ATb.size() != m_numCols*numRhs)
		m_ATb.resize(m_numCols*numRhs);

	// multiply right-hand side
	for (int i = 0; i < numRhs; ++i) {
		MulMatrixVector(m_AT, b + i*m_numRows, (taucsType *)&(m_ATb.front()) + i*m_numCols);
	}

	int rc;
	// solve the system
	rc = taucs_linsolve(m_ATA,
						&m_factorATA,
						numRhs,
						x,
						(taucsType *)&(m_ATb.front()),
						SIVANsolve,
						SIVANopt_arg);
	return (rc == TAUCS_SUCCESS); 
}

// Same as SolveATA only that this solves the actual system Ax = b
// It is assumed that A is rectangular and invertible
bool SparseSolver::SolveA(const taucsType * b, taucsType * x, const int numRhs) {
	if (m_factorA == NULL) 
		if (!FactorA())
			return false;

	// solve the system
	int	rc = taucs_linsolve(m_A,
							&m_factorA,
							numRhs,
							x,
							(void *)b,
							SIVANsolve,
							SIVANopt_arg);

	return (rc == TAUCS_SUCCESS);
}

// The version of SolveATA with 3 right-hand sides
// returns true on success, false otherwise
bool SparseSolver::SolveATA3(const taucsType * bx, const taucsType * by, const taucsType * bz,
							       taucsType * x,        taucsType * y,        taucsType * z) 
{
	if (m_factorATA == NULL) {
		bool rc = FactorATA();
		if (!rc)
			return false;
	}

	// prepare right-hand side
	if ((int)m_ATb.size() != m_numCols*3)
		m_ATb.resize(m_numCols*3);

	// multiply right-hand side
	MulMatrixVector(m_AT, bx, (taucsType *)&(m_ATb.front()));
	MulMatrixVector(m_AT, by, (taucsType *)&(m_ATb.front()) + m_numCols);
	MulMatrixVector(m_AT, bz, (taucsType *)&(m_ATb.front()) + 2*m_numCols);
	

	// solve the system
	int	rc = taucs_linsolve(m_ATA,
							&m_factorATA,
							1,
							x,
							(taucsType *)&(m_ATb.front()),
							SIVANsolve,
							SIVANopt_arg);

	if (rc != TAUCS_SUCCESS)
		return false;

	rc = taucs_linsolve(m_ATA,
							&m_factorATA,
							1,
							y,
							(taucsType *)&(m_ATb.front()) + m_numCols,
							SIVANsolve,
							SIVANopt_arg);

	if (rc != TAUCS_SUCCESS)
		return false;

	rc = taucs_linsolve(m_ATA,
							&m_factorATA,
							1,
							z,
							(taucsType *)&(m_ATb.front()) +  2*m_numCols,
							SIVANsolve,
							SIVANopt_arg);

	return (rc == TAUCS_SUCCESS);
}

// The version of SolveATA with 2 right-hand sides
// returns true on success, false otherwise
bool SparseSolver::SolveATA2(const taucsType * bx, const taucsType * by,
								   taucsType * x,        taucsType * y)
{
	if (m_factorATA == NULL) {
		bool rc = FactorATA();
		if (!rc)
			return false;
	}

	// prepare right-hand side
	if ((int)m_ATb.size() != m_numCols*2)
		m_ATb.resize(m_numCols*2);

	// multiply right-hand side
	MulMatrixVector(m_AT, bx, (taucsType *)&(m_ATb.front()));
	MulMatrixVector(m_AT, by, (taucsType *)&(m_ATb.front()) + m_numCols);
	

	// solve the system
	int	rc = taucs_linsolve(m_ATA,
							&m_factorATA,
							1,
							x,
							(taucsType *)&(m_ATb.front()),
							SIVANsolve,
							SIVANopt_arg);

	if (rc != TAUCS_SUCCESS)
		return false;

	rc = taucs_linsolve(m_ATA,
							&m_factorATA,
							1,
							y,
							(taucsType *)&(m_ATb.front()) + m_numCols,
							SIVANsolve,
							SIVANopt_arg);

	return (rc == TAUCS_SUCCESS);
}

void SparseSolver::ClearFactorATA() {
	// release the factor
	taucs_linsolve(NULL,&m_factorATA,0, NULL,NULL,SIVANfactor,SIVANopt_arg);
	m_factorATA = NULL;
	m_etree = NULL;
}

void SparseSolver::ClearFactorA() {
	// release the factor
	if (m_SPD)
		taucs_linsolve(NULL,&m_factorA,0, NULL,NULL,SIVANfactor,SIVANopt_arg);
	else
		taucs_linsolve(NULL,&m_factorA,0, NULL,NULL,SIVANfactorLU,SIVANopt_arg);
	m_factorA = NULL;

}

void SparseSolver::ClearMatricesATA() {
	// free the matrices
	taucs_free(m_AT);
	m_AT = NULL;
	taucs_free(m_ATA);
	m_ATA = NULL;
}

void SparseSolver::ClearMatricesA() {
	// free the matrices
	taucs_free(m_A);
	m_A = NULL;
}

// allows to add an anchored vertex without destroying the factor, if there was one
// i is the anchor's number (i.e. the index of the mesh vertex that is anchored is i)
// w is the weight of the anchor in the original Ax=b system. HAS TO BE POSITIVE!!
// if there was no factor for A^T A, this will simply add an anchor row to the Ax=b system
// if there was a factor, the factor will be updated, approximately at the cost of
// a single solve.
void SparseSolver::AddAnchor(const int i, const taucsType w) {
	if (m_SPD) {
		// update the columns
		if (m_colsA[i].find(i) == m_colsA[i].end())
			m_colsA[i][i] = w*w;
		else
			m_colsA[i][i] += w*w;

		if (m_factorA != NULL) {
			// the matrix is SPD so we update the factor of A
			chol_update(m_factorA, i, w, &m_etree);
			// update the matrix
			m_A->taucs_values[ m_A->colptr[i] ] += w*w;
		}
	}
	else {
		m_colsA[i][m_numRows] = w;
		m_numRows++;

		if (m_factorATA != NULL) {
			// update the ATA factor
			chol_update(m_factorATA, i, w, &m_etree);
			// update ATA
			m_ATA->taucs_values[ m_ATA->colptr[i] ] += w*w;
			// update A and AT (for multiplying rhs)
			CreateA();
			taucs_free(m_AT);
            m_AT = MatrixTranspose(m_A);
		}
	}	
}

void SparseSolver::MultiplyMatrixVector(const taucsType * v, taucsType * result, const int numCols) const {
	// make result all zero
	memset(result, 0, m_numRows * numCols * sizeof(taucsType));

	std::map<int,taucsType>::const_iterator iter;
	int										offset_v;
	int										offset_r;

	if (m_SPD) { // the matrix is symmetric
		for (int c = 0; c < numCols; ++c) {
			offset_v = m_numCols*c;
			for (int col = 0; col < m_numCols; ++col) {
				// going over column col of the matrix, multiplying
				// it by v[col] and setting the appropriate values
				// of vector result; also mirroring the other triangle
				for (iter = m_colsA[col].begin(); iter->first < col; ++iter) {
					result[iter->first + offset_v]	+= v[col + offset_v]*(iter->second);
					// mirroring
					result[col + offset_v]			+= v[iter->first + offset_v]*(iter->second);
				}
				if (iter->first == col)
					result[iter->first + offset_v]	+= v[col + offset_v]*(iter->second);
			}
		}
	}
	else {
		for (int c = 0; c < numCols; ++c) {
			offset_v = m_numCols*c;
			offset_r = m_numRows*c;
			for (int col = 0; col < m_numCols; ++col) {
				// going over column col of the matrix, multiplying
				// it by v[col] and setting the appropriate values
				// of vector result
				for (iter = m_colsA[col].begin(); iter != m_colsA[col].end(); ++iter) {
					result[iter->first + offset_r] += v[col + offset_v]*(iter->second);
				}
			}
		}
	}
}

// Multiplies the matrix by diagonal matrix D from the left, stores the result
// in the columns of res
void SparseSolver::MultiplyDiagMatrixMatrix(const taucsType * D, SparseSolver & res) const {
	res = SparseSolver(m_numRows, m_numCols, false);

	std::map<int,taucsType>::const_iterator iterA;
	std::map<int,taucsType>::iterator		iterRes;

	res.m_colsA = m_colsA;
	if (m_SPD) {
		for (int col = 0; col < m_numCols; ++col) {
			iterA = m_colsA[col].begin();
			iterRes = res.m_colsA[col].begin();
			for ( ; iterA != m_colsA[col].end() && iterA->first <= col; ++iterA, ++iterRes) {
				iterRes->second = (iterA->second) * D[iterA->first];
				// mirroring
				res.m_colsA[iterA->first][col] = (iterA->second) * D[col];
			}
		}
	}
	else {
		for (int col = 0; col < m_numCols; ++col) {
			iterA	= m_colsA[col].begin();
			iterRes = res.m_colsA[col].begin();
			for ( ; iterA != m_colsA[col].end(); ++iterA, ++iterRes) {
				iterRes->second = (iterA->second) * D[iterA->first];
			}
		}
	}
}

// Multiplies the matrix by the given matrix B from the right
// and stores the result in the columns of res
// isSPD tells us whether the result should be SPD or not
// retuns false if the dimensions didn't match (in such case res is unaltered), otherwise true
bool SparseSolver::MultiplyMatrixRight(const SparseSolver & B, SparseSolver & res,const bool isSPD) const {
	// for now ignore the option that one of the involved matrices is symmetric and doesn't have
	// stored under-diagonal values in m_colsA
	// we assume both matrices have all the values in m_colsA

	// (m x n) * (n x k)
	int m = m_numRows;
	int n = m_numCols;
	int k = B.m_numCols;

	if (B.m_numRows != n)
		return false;

	std::map<int,taucsType>::const_iterator iterBi, iterA;
	std::map<int,taucsType>::iterator iterRes;

	int			rowInd;
	taucsType	BiVal;

	res = SparseSolver(m, k, isSPD);
	for (int i = 0; i < k; ++i) { // go over the columns of res; res(i) = A*B(i)
		for (iterBi = B.m_colsA[i].begin(); iterBi != B.m_colsA[i].end(); ++iterBi) { // go over column B(i)
			// iterBi->second multiplies column A(iterBi->first)
			BiVal = iterBi->second;
			rowInd = iterBi->first;

			for (iterA = m_colsA[rowInd].begin(); iterA != m_colsA[rowInd].end(); ++iterA) { // go over column of A
				iterRes = res.m_colsA[i].find(iterA->first);
				if (iterRes == res.m_colsA[i].end()) // first occurence of this row number
					res.m_colsA[i][iterA->first] = (iterA->second)*BiVal;
				else
					iterRes->second += (iterA->second)*BiVal;
			}
		}
	}

	return true;
}

// Transposes the matrix 
// and stores the result in the columns of res
bool SparseSolver::Transpose(SparseSolver & res) const {
	res = SparseSolver(m_numCols, m_numRows, m_SPD);
	// if the matrix was symmetric to begin with, just copy the columns
	if (m_SPD) {
		res.m_colsA = m_colsA;
		return true;
	}
	// else need to transpose
	std::map<int,taucsType>::const_iterator iterA;
	for (int i=0; i < m_numCols; ++i) {
		for (iterA = m_colsA[i].begin(); iterA != m_colsA[i].end(); ++iterA) {
			res.m_colsA[iterA->first][i] = iterA->second;
		}
	}
	return true;
}

// will remove the rows startInd-endInd (including ends, zero-based) from the matrix
// will discard any factors	
bool SparseSolver::DeleteRowRange(const int startInd, const int endInd) {
	if (startInd < 0 || endInd >= m_numRows || startInd > endInd)
		return false;

	ClearFactorATA();
	ClearFactorA();
	ClearMatricesATA();
	ClearMatricesA();

	std::map<int,taucsType>::iterator it;
	std::map<int,taucsType>::iterator tempIt;
	int diff = endInd - startInd + 1;

	for (int c = 0; c < m_numCols; ++c) {
		it = m_colsA[c].begin();
		while (it != m_colsA[c].end()) {
			if (it->first >= startInd && it->first <= endInd) // row that should be erased
				it = m_colsA[c].erase(it);
			else {
				if (it->first > endInd) {// row number must be updated
					m_colsA[c][it->first - diff] = it->second;
					it = m_colsA[c].erase(it);
				}
				else
                    ++it;
			}
		}
	}

	m_numRows -= diff;
	m_SPD = false;
	
	return true;
}

bool SparseSolver::MultiplyRows(const int startInd, const int endInd, const double val) {
	if (startInd < 0 || endInd >= m_numRows || startInd > endInd)
		return false;

	ClearFactorATA();
	ClearFactorA();
	ClearMatricesATA();
	ClearMatricesA();

	std::map<int,taucsType>::iterator it;
		
	for (int c = 0; c < m_numCols; ++c) {
		it = m_colsA[c].begin();
		while (it != m_colsA[c].end()) {
			if (it->first >= startInd && it->first <= endInd) // row that should be multiplied
				it->second = it->second * val;
			it++;
		}
	}

	return true;
}


// will remove the columns startInd-endInd (including ends, zero-based) from the matrix
// will discard any factors	
bool SparseSolver::DeleteColumnRange(const int startInd, const int endInd) {
	if (startInd < 0 || endInd >= m_numCols || startInd > endInd)
		return false;

	ClearFactorATA();
	ClearFactorA();
	ClearMatricesATA();
	ClearMatricesA();

	m_colsA.erase(m_colsA.begin() + startInd, m_colsA.begin() + (endInd+1));

	m_numCols -= (endInd - startInd + 1);

	return true;
}

// copies columns startInd through endInd (including, zero-based) from the matrix to res matrix
// previous data in res, including factors, is erased.
bool SparseSolver::CopyColumnRange(const int startInd, const int endInd, SparseSolver & res) const {
	if (startInd < 0 || endInd >= m_numCols || startInd > endInd)
		return false;

	int k = endInd - startInd + 1;

	res = SparseSolver(m_numRows, k, false);
	for (int c = startInd; c <= endInd; ++c)
		res.m_colsA[c - startInd] = m_colsA[c];

	return true;
}

// copies rows startInd through endInd (including, zero-based) from the matrix to res matrix
// previous data in res, including factors, is erased.
bool SparseSolver::CopyRowRange(const int startInd, const int endInd, SparseSolver & res) const {
	if (startInd < 0 || endInd >= m_numRows || startInd > endInd)
		return false;

	res.m_colsA = m_colsA;
	for (int c = 0; c < m_numCols; ++c) {
		std::map<int,taucsType>::iterator it = res.m_colsA[c].begin();
		std::map<int,taucsType>::iterator tempIt;
		for ( ; it != res.m_colsA[c].end(); ++it) {
			if (it->first < startInd || it->first > endInd) {
				tempIt = it;
				it++;
				res.m_colsA[c].erase(tempIt);
				continue;
			}
		}
	}

	return true;
}



