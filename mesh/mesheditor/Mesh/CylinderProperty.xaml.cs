using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace mesheditor.Mesh
{
  /// <summary>
  /// Interaction logic for CylinderProperty.xaml
  /// </summary>
  public partial class CylinderProperty : UserControl
  {
    #region Constructors

    public CylinderProperty()
    {
      InitializeComponent();

      SubscribeToEvents();
    }

    #endregion

    #region Other Event Handlers

    private void inner_PreviewKeyUp(object sender, KeyEventArgs e)
    {
      if (e.Key == System.Windows.Input.Key.Enter)
      {
        float r = float.Parse(tbInner.Text.ToString());
        Globals.Manager.SetInnerRadius(r);
        Globals.MeshP.UpdateImage();
      }
    }

    private void outer_PreviewKeyUp(object sender, KeyEventArgs e)
    {
      if (e.Key == System.Windows.Input.Key.Enter)
      {
        float r = float.Parse(tbOuter.Text.ToString());
        Globals.Manager.SetOuterRadius(r);
        Globals.MeshP.UpdateImage();
      }
    }

    private void height_PreviewKeyUp(object sender, KeyEventArgs e)
    {
      if (e.Key == System.Windows.Input.Key.Enter)
      {
        float h = float.Parse(tbHeight.Text.ToString());
        Globals.Manager.SetHeight(h);
        Globals.MeshP.UpdateImage();
      }
    }

    #endregion

    #region Private Functions

    /// <summary>
    /// Subscribe to different events
    /// </summary>
    void SubscribeToEvents()
    {
      tbInner.PreviewKeyUp += new KeyEventHandler(inner_PreviewKeyUp);
      tbOuter.PreviewKeyUp += new KeyEventHandler(outer_PreviewKeyUp);
      tbHeight.PreviewKeyUp += new KeyEventHandler(height_PreviewKeyUp);
    }

    #endregion
  }
}
