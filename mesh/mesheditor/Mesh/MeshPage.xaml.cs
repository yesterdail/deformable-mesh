using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using mesheditor.Mesh.Helpers;

namespace mesheditor.Mesh
{
  /// <summary>
  /// Interaction logic for MeshPage.xaml
  /// </summary>
  public partial class MeshPage : IPage
  {
    #region Class Members

    bool mouseDown = false;
    Point oldMouse;

    #endregion

    #region Constructors

    public MeshPage()
    {
      InitializeComponent();

      SubscribeToEvents();
    }

    #endregion

    #region Overrides

    public bool Close()
    {
      Globals.Manager.Dispose();
      return true;
    }

    #endregion

    #region Mouse Event Handlers

    private void image_MouseDown(object sender, MouseButtonEventArgs e)
    {
      mouseDown = true;
      oldMouse = e.GetPosition(image);
    }

    private void image_MouseMove(object sender, MouseEventArgs e)
    {
      if (!mouseDown) return;

      if (e.LeftButton == MouseButtonState.Pressed)
      {
        Point point = e.GetPosition(image);
        Globals.Manager.Rotate((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
      else if (e.RightButton == MouseButtonState.Pressed)
      {
        Point point = e.GetPosition(image);
        Globals.Manager.Zoom((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
      else if (e.MiddleButton == MouseButtonState.Pressed)
      {
        Point point = e.GetPosition(image);
        Globals.Manager.Move((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
    }

    private void image_MouseUp(object sender, MouseButtonEventArgs e)
    {
      mouseDown = false;
    }

    #endregion

    #region Other Event Handlers

    private void UserControl_Loaded(object sender, System.Windows.RoutedEventArgs e)
    {
      int width = 0, height = 0;
      GetRegion(ref width, ref height);

      Globals.Manager = new hj.ManagerCLR();
      Globals.Manager.CreateView(width, height);
      Globals.Initialized = true;
      UserControl_SizeChanged(null, null);
    }

    private void UserControl_SizeChanged(object sender, SizeChangedEventArgs e)
    {
      if (Globals.Initialized)
      {
        int width = 0, height = 0;
        GetRegion(ref width, ref height);
        Globals.Manager.Resize(width, height);
        UpdateImage();
      }
    }

    private void btnImportMesh_Click(object sender, RoutedEventArgs e)
    {
      string meshfile = @"E:\samples\cactus.off";
      if (Globals.Manager.LoadMesh(meshfile))
      {
        UpdateImage();
      }
    }

    #endregion

    #region Private Functions

    /// <summary>
    /// Subscribe to different events
    /// </summary>
    void SubscribeToEvents()
    {
      this.Loaded += new System.Windows.RoutedEventHandler(UserControl_Loaded);
      this.SizeChanged += new SizeChangedEventHandler(UserControl_SizeChanged);
      btnImportMesh.Click += new RoutedEventHandler(btnImportMesh_Click);

      image.MouseDown += new MouseButtonEventHandler(image_MouseDown);
      image.MouseMove += new MouseEventHandler(image_MouseMove);
      image.MouseUp += new MouseButtonEventHandler(image_MouseUp);
    }

    /// <summary>
    /// Get the real Render image size.
    /// </summary>
    public void GetRegion(ref int width, ref int height)
    {
      width = (int)grid.ActualWidth;
      height = (int)grid.ActualHeight;
      width = width / 4 * 4;
    }

    /// <summary>
    /// Refresh Image
    /// </summary>
    public void UpdateImage()
    {
      Action action = new Action(delegate()
      {
        int width = 0, height = 0;
        GetRegion(ref width, ref height);

        int bytes = width * height * 3;
        byte[] buffer = new byte[bytes];
        Globals.Manager.GetView(buffer);
        image.Source = ByteImageConverter.ConvertFrom(buffer, width, height);
      });
      action.Invoke();
    }
    #endregion
  }
}
