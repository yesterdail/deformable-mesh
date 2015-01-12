#ifndef HJ_MeshRenderer_h__
#define HJ_MeshRenderer_h__

#include "common/macro.h"
#include "common/glmext.h"
#include "common/TriMesh.h"
#include "common/Camera.h"
#include "common/Trackball.h"

namespace hj
{
  class GLFramebuffer;
  class GLProgram;
  class Image;
  class PCA;
  class LaplacianSurface;
  class GraphicsRenderer;

  struct Cylinder
  {
    float inner_radius;
    float outer_radius;
    float height;
    glm::vec3 center_world;
    glm::mat4 model_matrix;
  };

  class MeshRenderer
  {
  public:
    /**
    * Default constructor
    */
    MeshRenderer();

    /**
    * Destructor.
    */
    ~MeshRenderer();

    /**
    * Set FBO
    * @return: True if data are valid, false otherwise.
    */
    bool SetFBO(GLFramebuffer* fbo);

    /**
    * Set associate class.
    * @return: True if data are valid, false otherwise.
    */
    void SetAssociate(GraphicsRenderer* gren);

    /**
    * Paint.
    */
    bool Run();

    /**
    * Loads a mesh.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadMesh(const std::string& filename);

    /**
    * Loads a texture.
    * @param file_paths: path of mesh.
    * @return: True if all parameters are valid, false otherwise.
    */
    bool LoadTexture(const std::string& filename);

    /**
    * Resets positions/directions of the camera.
    * @return: True if successful, false otherwise.
    */
    bool ResetCamera();

    /**
    * Rotates VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Rotate(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Zoom VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Zoom(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Move VR.
    * @param newMouseX: current mouse position X in view coordinate.
    * @param newMouseY: current mouse position Y in view coordinate.
    * @param lastMouseX: last mouse position X in view coordinate.
    * @param lastMouseY: last mouse position Y in view coordinate.
    */
    void Move(float newMouseX,
      float newMouseY,
      float lastMouseX,
      float lastMouseY);

    /**
    * Set smooth rendering.
    */
    void SetSmooth();

    /**
    * Set flat rendering.
    */
    void SetFlat();

    /**
    * Set wire frame rendering.
    */
    void SetWireframe(bool w);

    /**
    * Set solid rendering.
    */
    void SetSolid(bool s);

    /**
    * Set rendering with texture.
    */
    void SetTexture(bool t);

    /**
    * Set anchor points.
    * @param polygon: polygon points array.
    */
    void SetAnchorPoints(const std::vector<glm::vec2> &polygon);

    /**
    * Set control points.
    * @param polygon: polygon points array.
    */
    void SetControlPoints(const std::vector<glm::vec2> &polygon);

    /**
    * actions after selection.
    * @param point: mouse point.
    */
    bool PostSelection(const glm::vec2 &point);

    /**
    * mesh deformation.
    * @param point: mouse point.
    */
    bool Deformation(const glm::vec2 &point);

    /**
    * Cancel deformation.
    */
    void CancelDeform();

    /**
    * restore mesh to original.
    */
    void RestoreMesh();

    /**
    * Cut mesh accordint to the line.
    */
    void CutMesh();

    /**
    * Add a default cylinder at the center of (mouseX, mouseY).
    * @param mouseX: current mouse position X in view coordinate.
    * @param mouseY: current mouse position Y in view coordinate.
    */
    void AddCylinder(float mouseX, float mouseY);


    TriMesh* GetMesh() { return mesh_; }

    std::vector<TriMesh::VHandle>& GetControlPts() { return controlPts_; }

    std::vector<TriMesh::VHandle>& GetAnchorPts() { return anchorPts_; }

  private:

    /**
    * draw the loaded mesh.
    */
    void drawMainObject(float r, float g, float b);

    /**
    * draw anchor points as red spheres, control points as green spheres
    */
    void drawAnchorAndControl();

    /**
    * draw region-of-interest.
    */
    void drawROI(double r, double g, double b);

    /**
    * get ROI region from lasso 2d
    * project vertices to camera coordinate system, then project these points to near plane,
    * then check if they're inside of the lasso lines (which is a polygon).
    * check if point is inside of polygon
    * http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/isInsideOfPolygon.html
    * @param polygon: lasso3d point set.
    */
    void getLasso2dRegion(const std::vector<glm::vec2> &polygon);

    /**
    * vertex set -> face set
    */
    void vertex2Face(std::vector<TriMesh::VHandle> &vset,
      std::vector<TriMesh::FHandle> &bfs);

    /**
    * face set -> vertex set
    */
    void face2Vertex(std::vector<TriMesh::VHandle> &vset,
      std::vector<TriMesh::FHandle> &fset);

    /**
    * Transform point form view coordiante to model coordinate.
    * @param point: point in view coordinate.
    * @return: point in model coordinate.
    */
    Vec View2Model(const Vec &point);

    /**
    * Transform point form model coordiante to view coordinate.
    * @param point: point in model coordinate.
    * @return: point in view coordinate.
    */
    Vec Model2View(const Vec &point);

    /**
    * Transform point form view coordiante to world coordinate.
    * @param point: point in view coordinate.
    * @return: point in world coordinate.
    */
    Vec View2World(const Vec &point);

    /**
    * Transform point form world coordiante to view coordinate.
    * @param point: point in world coordinate.
    * @return: point in view coordinate.
    */
    Vec World2View(const Vec &point);

  private:
    /** Output frame buffer. */
    GLFramebuffer* out_fbo_ptr_;

    /** input mesh. */
    TriMesh* mesh_;

    /** mesh center in world coordinate. */
    Point center_;

    /** mesh radius in world coordinate. */
    float radius_;

    /** All vertices for drawing. */
    std::vector<unsigned int> triverts_;

    /** ROI vertices for drawing. */
    std::vector<unsigned int> roiverts_;

    PerspectiveCamera camera_;
    Trackball trackball_;
    glm::mat4 model_;

    /** fov y in degree. */
    const float fovy_;

    /** wire frame rendering. */
    bool wireframe_;

    /** solid rendering. */
    bool solid_;

    /** rendering with texture. */
    bool texture_;

    Image* texture_image_;

    std::vector<TriMesh::FHandle> curFRoi_; // current bfs result, face list
    std::vector<TriMesh::FHandle> allFRoi_; // all Roi with boolean operations in every step, face list
    std::vector<TriMesh::VHandle> allVRoi_; // all Roi, vertex list

    std::vector<TriMesh::VHandle> controlPts_; // control points, for deformation
    std::vector<TriMesh::VHandle> anchorPts_; // anchor points, for deformation

    PCA* pcaAnchor_;
    PCA *pcaControl_;

    bool isPreComputed_; // is precomputed (matrix factorization) or not

    Vec curPointInWorld_; // current point in world coordinate system (picked by mouse)
    Vec initialPointInCam_; // initial point in camera coordiante system, need the correct depth value later
    Vec movingPointInWorld_; // point selected during moving (in world coordinate system)
    TriMesh::Point translationInWorld_; // translation vector for all control points (in world coordinate system)

    LaplacianSurface *ls_;
    int ARAPIteration_; // ARAP's iteration times. 0 means naive LSE

    /** hold mesh file name for mesh restore. */
    std::string meshfile_;

    /** depth buffer. */
    float* depth_buffer_;

    GraphicsRenderer* gren_;

    /** cylinder array. */
    std::vector<Cylinder> cylinders_;
  };
}
#endif // HJ_MeshRenderer_h__