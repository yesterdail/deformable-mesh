#ifndef HJ_GLOffScreenRender_h__
#define HJ_GLOffScreenRender_h__


namespace hj
{
  class GLOffScreenRender
  {
  public:
    GLOffScreenRender();
    ~GLOffScreenRender();

    /**
    * Makes the OpenGL rendering context within this manager the current RC.
    */
    bool MakeCurrent();

    /**
    * Makes current OpenGL rendering context within this manager no longer current.
    */
    bool MakeUncurrent();

    /*
    * Initializes rendering context.
    * @return: True if successful, false otherwise.
    */
    bool InitializeContext();

    /**
    * Creates a hidden window for off-screen rendering.
    * @return: True if successful, false otherwise.
    */
    bool CreateHiddenWindow();

    /**
    * Destroys rendering context of this manager.
    * @return: True if successful, false otherwise.
    */
    bool DestroyContext();

    /**
    * Destroys the hidden window.
    * @return: True if the window and related resources are released properly, false otherwise.
    */
    bool DestroyHiddenWindow();

  private:

    /** Window handle. */
    void *window_handle_;

    /** DC handle to the window. */
    void *dc_handle_;

    /** Rendering context handle. */
    void *rc_handle_;

    /** Flag indicating if the window handle is passed to this object or owned by it. */
    bool own_window_;
  };
}

#endif // HJ_GLOffScreenRender_h__
