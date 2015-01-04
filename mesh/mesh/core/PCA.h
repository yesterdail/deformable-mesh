#ifndef HJ_PCA_h__
#define HJ_PCA_h__

#include "common/TriMesh.h"
#include "newmatap.h" // need matrix applications

namespace hj
{
  class PCA
  {
  public:
    PCA();
    ~PCA();

    void SetMesh(TriMesh* mesh);

    //---------------------------------------------------------------------------------------------
    /**
    * calculates principal component of region-of-interest, and get a axis and
    * oriented bounding box from the PCA.
    */

    /**
    * find centroid of PCA (allVRoi)
    */
    void getPCACentroid(std::vector<TriMesh::VHandle> &allVRoi);

    /**
    * find PCA axis and PCA oriented bounding box of region-of-interest
    * using svd (UDV' = UDU') to get PCA. the left column of U is principal component
    * Three vectors in U will be the axis
    */
    void getPCAAxis(std::vector<TriMesh::VHandle> &allVRoi);

    /**
    * find oriented bounding box (obb) with pca axis.
    * Step 1: transform axis to original coordinate system, denote this transformation matrix as T
    * Step 2: Apply T on all vertices in PCA. Now PCA are placed in original coordinate system
    * Step 3: Get bouding box (unoriented) in the original coordinate system
    * Step 4: Transform bounding box back using inverse matrix of T, now it's OBB
    */
    void getPCAOBB(std::vector<TriMesh::VHandle> &allVRoi);

    /**
    * draw 8 blue faces with GL_BLEND, then draw 8 green lines
    */
    void drawPCAOBB();

    /**
    * draw a sphere for all control points, use can click the sphere and deform the control points
    */
    void drawControlSphere();

    Matrix getPCATransformation();

    void addOneOBBPoint(double x, double y, double z, Matrix &transformation, ColumnVector & pt);

    void getControlSphere(std::vector<TriMesh::VHandle> &controlPts);

    Vec getCentroid();

    TriMesh::Scalar getRadius() { return radius_; }

  private:
    TriMesh* mesh_;

    // PCA
    Point centroid_; // center of PCA
    std::vector<Point> axis_; // principal component of Roi
    std::vector<Point> obb_; // bounding box of Roi, calculated from pca axis
    TriMesh::Scalar radius_; // radius for control sphere
  };
}


#endif // HJ_PCA_h__