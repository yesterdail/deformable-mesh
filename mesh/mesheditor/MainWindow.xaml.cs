using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Interop;
using mesheditor.Metro.Native;

namespace mesheditor
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    #region Constructors

    public MainWindow()
    {
      InitializeComponent();

      SubscribeToEvents();

      Window_StateChanged(null, null);
    }

    #endregion

    #region Mouse Event Handlers

    private void headerThumb_DragDelta(object sender, System.Windows.Controls.Primitives.DragDeltaEventArgs e)
    {
      Left = Left + e.HorizontalChange;
      Top = Top + e.VerticalChange;
    }

    private void headerThumb_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      if (this.WindowState == System.Windows.WindowState.Normal)
        this.WindowState = System.Windows.WindowState.Maximized;
      else if (this.WindowState == System.Windows.WindowState.Maximized)
        this.WindowState = System.Windows.WindowState.Normal;
    }

    #endregion

    #region Other Event Handlers

    private void btnActionSupport_Click(object sender, RoutedEventArgs e)
    {
      // Load support page?
    }

    private void btnActionMinimize_Click(object sender, RoutedEventArgs e)
    {
      this.WindowState = System.Windows.WindowState.Minimized;
    }

    private void btnActionRestore_Click(object sender, RoutedEventArgs e)
    {
      this.WindowState = System.Windows.WindowState.Normal;
    }

    private void btnActionMaxamize_Click(object sender, RoutedEventArgs e)
    {
      this.WindowState = System.Windows.WindowState.Maximized;
    }

    private void btnActionClose_Click(object sender, RoutedEventArgs e)
    {
      Application.Current.Shutdown();
    }

    private void homeTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {

    }

    private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      meshPage.Close();
    }

    private void Window_StateChanged(object sender, EventArgs e)
    {
      if (this.WindowState == System.Windows.WindowState.Normal)
      {
        borderFrame.BorderThickness = new Thickness(1, 1, 1, 23);
        btnActionRestore.Visibility = System.Windows.Visibility.Collapsed;
        btnActionMaxamize.Visibility = System.Windows.Visibility.Visible;
      }
      else if (this.WindowState == System.Windows.WindowState.Maximized)
      {
        borderFrame.BorderThickness = new Thickness(0, 0, 0, 23);
        btnActionRestore.Visibility = System.Windows.Visibility.Visible;
        btnActionMaxamize.Visibility = System.Windows.Visibility.Collapsed;
      }
    }

    #endregion

    #region Private Functions

    /// <summary>
    /// Subscribe to different events
    /// </summary>
    void SubscribeToEvents()
    {
      this.Closing += new System.ComponentModel.CancelEventHandler(Window_Closing);
      this.StateChanged += new EventHandler(Window_StateChanged);

      headerThumb.DragDelta += new DragDeltaEventHandler(headerThumb_DragDelta);
      headerThumb.MouseDoubleClick += new MouseButtonEventHandler(headerThumb_MouseDoubleClick);

      // Window buttons
      btnActionSupport.Click += new RoutedEventHandler(btnActionSupport_Click);
      btnActionMinimize.Click += new RoutedEventHandler(btnActionMinimize_Click);
      btnActionRestore.Click += new RoutedEventHandler(btnActionRestore_Click);
      btnActionMaxamize.Click += new RoutedEventHandler(btnActionMaxamize_Click);
      btnActionClose.Click += new RoutedEventHandler(btnActionClose_Click);

      // Tab Control
      homeTabControl.SelectionChanged += new SelectionChangedEventHandler(homeTabControl_SelectionChanged);
    }

    #endregion

    #region Visual Children Overrides

    protected override void OnSourceInitialized(EventArgs e)
    {
      base.OnSourceInitialized(e);

      var handle = (new WindowInteropHelper(this)).Handle;
      var hwndSource = HwndSource.FromHwnd(handle);
      if (hwndSource != null) hwndSource.AddHook(WindowProc);
    }

    #endregion

    #region Maximize Workspace Workarounds
    private System.IntPtr WindowProc(
          System.IntPtr hwnd,
          int msg,
          System.IntPtr wParam,
          System.IntPtr lParam,
          ref bool handled)
    {
      switch (msg)
      {
        case 0x0024:
          WmGetMinMaxInfo(hwnd, lParam);
          handled = true;
          break;
      }

      return (System.IntPtr)0;
    }
    private void WmGetMinMaxInfo(System.IntPtr hwnd, System.IntPtr lParam)
    {
      Monitor_Workarea.MINMAXINFO mmi = (Monitor_Workarea.MINMAXINFO)Marshal.PtrToStructure(lParam, typeof(Monitor_Workarea.MINMAXINFO));

      // Adjust the maximized size and position to fit the work area of the correct monitor
      int MONITOR_DEFAULTTONEAREST = 0x00000002;
      System.IntPtr monitor = Monitor_Workarea.MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

      if (monitor != System.IntPtr.Zero)
      {
        System.Windows.Forms.Screen scrn = System.Windows.Forms.Screen.FromHandle(new WindowInteropHelper(this).Handle);

        Monitor_Workarea.MONITORINFO monitorInfo = new Monitor_Workarea.MONITORINFO();
        Monitor_Workarea.GetMonitorInfo(monitor, monitorInfo);
        Monitor_Workarea.RECT rcWorkArea = monitorInfo.rcWork;
        Monitor_Workarea.RECT rcMonitorArea = monitorInfo.rcMonitor;
        mmi.ptMaxPosition.x = Math.Abs(rcWorkArea.left - rcMonitorArea.left);
        mmi.ptMaxPosition.y = Math.Abs(rcWorkArea.top - rcMonitorArea.top);
        mmi.ptMaxSize.x = Math.Abs(rcWorkArea.right - rcWorkArea.left);
        mmi.ptMaxSize.y = Math.Abs(rcWorkArea.bottom - rcWorkArea.top);

        /*
        mmi.ptMaxPosition.x = Math.Abs(scrn.Bounds.Left - scrn.WorkingArea.Left);
        mmi.ptMaxPosition.y = Math.Abs(scrn.Bounds.Top - scrn.WorkingArea.Top);
        mmi.ptMaxSize.x = Math.Abs(scrn.Bounds.Right - scrn.WorkingArea.Left);
        mmi.ptMaxSize.y = Math.Abs(scrn.Bounds.Bottom - scrn.WorkingArea.Top);
        */
      }

      Marshal.StructureToPtr(mmi, lParam, true);
    }
    #endregion
  }
}
