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
    bool onDeformation = false;

    // Dependency properties
    public static readonly DependencyProperty ToolTypeProperty;

    #endregion

    #region Constructors

    public MeshPage()
    {
      InitializeComponent();

      SubscribeToEvents();
    }

    static MeshPage()
    {
      PropertyMetadata metaData;

      metaData = new PropertyMetadata(ToolType.Pointer);
      ToolTypeProperty = DependencyProperty.Register(
          "Type", typeof(ToolType), typeof(MeshPage),
          metaData);
    }

    #endregion

    #region Dependency Properties

    public ToolType Type
    {
      get { return (ToolType)GetValue(ToolTypeProperty); }
      set
      {
        if ((int)value >= 0 && (int)value < (int)ToolType.Max)
        {
          SetValue(ToolTypeProperty, value);
          Globals.Manager.SetToolType((int)Type);
        }
      }
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

    private void grid_MouseDown(object sender, MouseButtonEventArgs e)
    {
      mouseDown = true;

      Point point = e.GetPosition(image);
      oldMouse = point;

      // tool operation.
      if (e.ChangedButton == MouseButton.Left)
      {
        if (Globals.Manager.OnMouseDown_GraphicsOverlay((float)point.X, (float)point.Y))
        {
          UpdateImage();
          return;
        }
      }

      
      if (Globals.Manager.PostSelection((float)point.X, (float)point.Y))
      {
        onDeformation = true;
        return;
      }

      // cylinder property
      if (e.ChangedButton == MouseButton.Right)
      {
        if(Globals.Manager.CheckSelection((float)point.X, (float)point.Y))
        {
          UpdateImage();
          float[] prop = new float[3];
          if (Globals.Manager.GetSelectionProperty(prop))
          {
            cyProp.Margin = new Thickness(point.X, point.Y, 0, 0);
            cyProp.tbInner.Text = prop[0].ToString();
            cyProp.tbOuter.Text = prop[1].ToString();
            cyProp.tbHeight.Text = prop[2].ToString();
            cyProp.Visibility = Visibility.Visible;
            return;
          }
        }
      }

      // selection
      if(Globals.Manager.CheckSelection((float)point.X, (float)point.Y))
      {
        UpdateImage();
      }

      cyProp.Visibility = Visibility.Collapsed;
    }

    private void grid_MouseMove(object sender, MouseEventArgs e)
    {
      if (!mouseDown) return;

      Point point = e.GetPosition(image);

      // tool operation.
      if (Globals.Manager.OnMouseMove_GraphicsOverlay((float)point.X, (float)point.Y))
      {
        UpdateImage();
        return;
      }

      if (onDeformation)
      {
        Globals.Manager.Deformation((float)point.X, (float)point.Y);
        UpdateImage();
        return;
      }

      if (e.LeftButton == MouseButtonState.Pressed)
      {
        Globals.Manager.Rotate((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
      else if (e.RightButton == MouseButtonState.Pressed)
      {
        Globals.Manager.Zoom((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
      else if (e.MiddleButton == MouseButtonState.Pressed)
      {
        Globals.Manager.Move((float)point.X, (float)point.Y, (float)oldMouse.X, (float)oldMouse.Y);
        UpdateImage();
        oldMouse = point;
      }
    }

    private void grid_MouseUp(object sender, MouseButtonEventArgs e)
    {
      // tool operation.
      if (e.ChangedButton == MouseButton.Left)
      {
        Point point = e.GetPosition(grid);
        if (Globals.Manager.OnMouseUp_GraphicsOverlay((float)point.X, (float)point.Y))
        {
          UpdateImage();
        }
      }

      mouseDown = false;
      onDeformation = false;
      Type = ToolType.Pointer;
    }

    private void btnAddCylinder_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      DataObject dataObject = new DataObject(0);
      DragDrop.DoDragDrop((DependencyObject)e.Source, dataObject, DragDropEffects.Copy);
    }

    private void image_Drop(object sender, DragEventArgs e)
    {
      Point p = e.GetPosition(image);
      Globals.Manager.AddCylinder((float)p.X, (float)p.Y);
      UpdateImage();
    }

    private void btnAddCylinder_GiveFeedback(object sender, GiveFeedbackEventArgs e)
    {
      Mouse.SetCursor(CursorHelper.CreateCursor(btnAddCylinder, 5, 5));
      e.Handled = true;
    }

    #endregion

    #region Other Event Handlers

    private void UserControl_Loaded(object sender, System.Windows.RoutedEventArgs e)
    {
      int width = 0, height = 0;
      GetRegion(ref width, ref height);

      Globals.MeshP = this;
      Globals.Manager = new hj.ManagerCLR();
      Globals.Manager.CreateView(width, height);
      Globals.Initialized = true;
      UserControl_SizeChanged(null, null);

      bool w = false, s = true, t = false;
      Globals.Manager.SetWireframe(w);
      Globals.Manager.SetSolid(s);
      Globals.Manager.SetTexture(t);
      btnWireframe.IsChecked = w;
      btnSolid.IsChecked = s;
      btnTexture.IsChecked = t;

      

      //slider.Minimum = 0.9;
      //slider.Maximum = 1;
      //slider.TickFrequency = 0.0001;
      //slider.Value = Globals.Manager.GetLineDepth();
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
      Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
      dlg.InitialDirectory = @"c:\";
      dlg.RestoreDirectory = true;
      dlg.Title = "Open Mesh";
      dlg.Filter = "OBJ Files (*.obj)|*.obj|PLY Files (*.ply)|*.ply|OFF Files (*.off)|*.off|STL Files (*.stl)|*.stl|All known formats(*.*)|*.*";
      dlg.ShowDialog();
      if (dlg.FileName.Length > 0)
      {
        string meshfile = dlg.FileName;
        //string meshfile = @"E:\samples\cactus.off";
        if (Globals.Manager.LoadMesh(meshfile))
        {
          UpdateImage();
        }
      }
    }

    private void btnImportTexture_Click(object sender, RoutedEventArgs e)
    {
      Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
      dlg.InitialDirectory = @"c:\";
      dlg.RestoreDirectory = true;
      dlg.Title = "Open texture file";
      dlg.Filter = "BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|All known formats(*.*)|*.*";
      dlg.ShowDialog();
      if (dlg.FileName.Length > 0)
      {
        string texfile = dlg.FileName;
        //string texfile = @"E:\samples\hj.bmp";
        if (Globals.Manager.LoadTexture(texfile))
        {
          Globals.Manager.SetTexture(true);
          btnTexture.IsChecked = true;
          UpdateImage();
        }
      }
    }

    private void btnWireframe_Click(object sender, RoutedEventArgs e)
    {
      bool w = (btnWireframe.IsChecked == true);
      Globals.Manager.SetWireframe(w);
      UpdateImage();
    }

    private void btnSmooth_Click(object sender, RoutedEventArgs e)
    {
      bool s = (btnSolid.IsChecked == true);
      Globals.Manager.SetSolid(s);
      UpdateImage();
    }

    private void btnTexture_Click(object sender, RoutedEventArgs e)
    {
      bool t = (btnTexture.IsChecked == true);
      Globals.Manager.SetTexture(t);
      UpdateImage();
    }

    private void ToolType_PreviewMouseDown(object sender, MouseButtonEventArgs e)
    {
      Type = (ToolType)Enum.Parse(typeof(ToolType),
        ((System.Windows.Controls.Primitives.ButtonBase)sender).Tag.ToString());
      e.Handled = true;
    }

    private void btnDelete_Click(object sender, RoutedEventArgs e)
    {
      Globals.Manager.RemoveAllGraphics();
      UpdateImage();
    }

    private void btnCancel_Click(object sender, RoutedEventArgs e)
    {
      Globals.Manager.CancelDeform();
      UpdateImage();
    }

    private void btnRestore_Click(object sender, RoutedEventArgs e)
    {
      Globals.Manager.RestoreMesh();
      UpdateImage();
    }

    private void btnCut_Click(object sender, RoutedEventArgs e)
    {
      Globals.Manager.CutMesh();
      UpdateImage();
    }

    private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      //Globals.Manager.SetLineDepth((float)slider.Value);
      //UpdateImage();
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
      btnImportTexture.Click += new RoutedEventHandler(btnImportTexture_Click);
      btnWireframe.Click += new RoutedEventHandler(btnWireframe_Click);
      btnSolid.Click += new RoutedEventHandler(btnSmooth_Click);
      btnTexture.Click += new RoutedEventHandler(btnTexture_Click);

      btnToolPointer.PreviewMouseDown += new MouseButtonEventHandler(ToolType_PreviewMouseDown);
      btnToolLine.PreviewMouseDown += new MouseButtonEventHandler(ToolType_PreviewMouseDown);
      btnCut.Click += new RoutedEventHandler(btnCut_Click);
      //btnDelete.Click += new RoutedEventHandler(btnDelete_Click);
      
      //btnToolAnchor.PreviewMouseDown += new MouseButtonEventHandler(ToolType_PreviewMouseDown);
      //btnToolControl.PreviewMouseDown += new MouseButtonEventHandler(ToolType_PreviewMouseDown);
      //btnCancel.Click += new RoutedEventHandler(btnCancel_Click);
      //btnRestore.Click += new RoutedEventHandler(btnRestore_Click);

      grid.MouseDown += new MouseButtonEventHandler(grid_MouseDown);
      grid.MouseMove += new MouseEventHandler(grid_MouseMove);
      grid.MouseUp += new MouseButtonEventHandler(grid_MouseUp);

      //slider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);

      btnAddCylinder.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(btnAddCylinder_PreviewMouseLeftButtonDown);
      btnAddCylinder.GiveFeedback += new GiveFeedbackEventHandler(btnAddCylinder_GiveFeedback);
      image.Drop += new DragEventHandler(image_Drop);
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
