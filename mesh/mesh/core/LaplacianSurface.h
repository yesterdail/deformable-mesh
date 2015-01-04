#ifndef HJ_LaplacianSurface_h__
#define HJ_LaplacianSurface_h__

#include "common/macro.h"
#include "common/TriMesh.h"
#include "newmatap.h" // need matrix applications
#include "taucs_interface.h"


/**
* This class is for Laplacian Surface Editing/Optimization algorithms.
* LSE is proposed by Olga Sorkine in "Laplacian Surface Editing"
* LSO is proposed by Andrew Nealen in "Laplacian Mesh Optimization"
* ARAPDeform is proposed by Olga Sorkine in "As-Rigid-As-Possible
* Shape Modeling".
*/
namespace hj
{
  class MeshRenderer;

  class LaplacianSurface
  {
  public:
    LaplacianSurface(MeshRenderer* ren);
    ~LaplacianSurface(void);

    // Laplacian Surface Optimization with uniform weights, Lu X = 0, or with cotangent weights, Lu X = b
    // cotangent weight, wij = cot(alpha) + cot(beta)
    // Figure 4 in Laplacian Surface Optimization by Andrew Nealen
    // smoothLSOAnchor is the percentage of anchor points
    void LSOLuXB();
    // pre compute Lu matrices and do sparse Cholesky factorization (using Taucs)
    // the size of Lu is number of ROI + number of control points + number of anchor points by number of ROI
    void PreCompute();
    // perform deformation
    void ARAPDeform(int ARAPIteration);
    // Find rigid rotation for every vertex by SVD, and stored into vector R
    // Notation is the same as "Least-Squares Rigid Motion Using SVD" by Olga Sorkine
    // xyz: new positions (solutions from previous step, x,y,z coordinates); 
    void SVDRotation();
    // translate all control points
    void translationDeform(TriMesh::Point &translation);
    // rotate all control points along (dir-centroid) axis
    void rotationDeform(double angle, TriMesh::Point centroid, TriMesh::Point dir);
    // pre-compute. calculate cotangent wij for each vertex pari(i,j). wij = 1/2 * (cot alpha + cot beta)
    void computeCotWij();

  private:
    // matrix id
    int Lu; // for LSO smooth
    int Lc; // for ARAP shape modeling
    std::vector<Matrix> R; // rotation matrix
    int *ctrlmark; // control points' vidmark
    TriMesh::Scalar *wijAll; // cotangent weight matrix
    taucsType *b3; // b matrix
    taucsType *xyz; // solution matrix
    taucsType *OrigMesh; // copy original positions
    Matrix eye; // identity matrix, defined here since it's frequently used

    int smoothLSOWeight;
    int smoothLSOAnchor; // smooth parameters

    MeshRenderer* ren_;
  };
}

#endif // HJ_LaplacianSurface_h__