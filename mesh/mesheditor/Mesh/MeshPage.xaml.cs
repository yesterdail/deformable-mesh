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

    #region Other Event Handlers

    private void UserControl_Loaded(object sender, System.Windows.RoutedEventArgs e)
    {
      Globals.Manager = new hj.ManagerCLR();
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

    #endregion

    #region Private Functions

    /// <summary>
    /// Subscribe to different events
    /// </summary>
    void SubscribeToEvents()
    {
      this.Loaded += new System.Windows.RoutedEventHandler(UserControl_Loaded);
      this.SizeChanged += new SizeChangedEventHandler(UserControl_SizeChanged);
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
