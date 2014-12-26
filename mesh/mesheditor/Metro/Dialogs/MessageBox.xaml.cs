using mesheditor.Metro.Native;
using System;
using System.Windows;
using System.Windows.Controls.Primitives;

namespace mesheditor.Metro.Dialogs
{
  /// <summary>
  /// Interaction logic for MessageBox.xaml
  /// </summary>
  public partial class MessageBox : Window
  {
    public MessageBox(string title, string message)
    {
      InitializeComponent();
      DwmDropShadow.DropShadowToWindow(this);

      lblTitle.Text = title;
      lblSubInfo.Text = message;
    }

    private void btnOkay_Click(object sender, System.Windows.RoutedEventArgs e)
    {
      this.Close();
    }
    private void headerThumb_DragDelta(object sender, DragDeltaEventArgs e)
    {
      Left = Left + e.HorizontalChange;
      Top = Top + e.VerticalChange;
    }
  }
}
