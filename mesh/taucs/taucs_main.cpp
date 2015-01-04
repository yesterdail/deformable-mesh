#include <taucs_interface.h>

void main() {
	InitTaucsInterface();

	int idA, idB;

	idA = CreateMatrix(4, 3);
	idB = CreateMatrix(3, 2);

	SetMatrixEntry(idA, 0, 0, 1);
	SetMatrixEntry(idA, 1, 1, 2);
	SetMatrixEntry(idA, 2, 2, 3);
	SetMatrixEntry(idA, 3, 0, 1);

	SetMatrixEntry(idB, 0, 0, -1);
	SetMatrixEntry(idB, 1, 1, -2);
	SetMatrixEntry(idB, 2, 0, 1);

	FactorATA(idA);
	FactorATA(idB);

	taucsType b1[4] = {3, 4, 5, 6};
	taucsType b2[3] = {3, 4, 5};

	taucsType x1[3];
	taucsType x2[2];

	SolveATA(idA, b1, x1, 1);
	SolveATA(idB, b2, x2, 1);

	taucsType c1[8] = {3, 4, 5, 6, 7, 8, 9, 10};
	taucsType xy1[6];

	SolveATA(idA, c1, xy1, 2);
	
	taucsType x11[3];

	SolveATA2(idA, b1, b1, x1, x11);

	ReleaseMatrix(idA);

	idA = CreateMatrix(2, 2);
	SetMatrixEntry(idA, 0, 0, -1);
	SetMatrixEntry(idA, 1, 1, -2);

	FactorATA(idA);

	taucsType bb[2] = {1,2};
	taucsType xx[2];

	SolveATA(idA, bb, xx, 1);

	DeinitTaucsInterface();
}