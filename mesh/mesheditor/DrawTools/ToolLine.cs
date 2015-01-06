using System;
using System.IO;
using System.Windows;
using System.Windows.Input;
using mesheditor.Mesh;

namespace mesheditor.DrawTools
{
  /// <summary>
  /// Line tool
  /// </summary>
  class ToolLine : ToolObject
  {
    GraphicsLine obj = null;

    public ToolLine()
    {
      MemoryStream stream = new MemoryStream(Properties.Resources.Line);
      ToolCursor = new Cursor(stream);
    }

    /// <summary>
    /// Create new object
    /// </summary>
    public override bool OnMouseDown(DrawingCanvas drawingCanvas, MouseButtonEventArgs e)
    {
      HelperFunctions.DeleteAll(drawingCanvas);
      Point p = e.GetPosition(drawingCanvas);
      obj = new GraphicsLine(
          p,
          new Point(p.X + 1, p.Y + 1),
          drawingCanvas.LineWidth,
          drawingCanvas.ObjectColor);
      AddNewObject(drawingCanvas, obj);

      return true;
    }

    /// <summary>
    /// Set cursor and resize new object.
    /// </summary>
    public override bool OnMouseMove(DrawingCanvas drawingCanvas, MouseEventArgs e)
    {
      drawingCanvas.Cursor = ToolCursor;

      if (e.LeftButton == MouseButtonState.Pressed)
      {
        drawingCanvas[drawingCanvas.Count - 1].MoveHandleTo(
            e.GetPosition(drawingCanvas), 2);

        return true;
      }

      return false;
    }

    /// <summary>
    /// Left mouse is released.
    /// New object is created and resized.
    /// </summary>
    public override bool OnMouseUp(DrawingCanvas drawingCanvas, MouseButtonEventArgs e)
    {
      base.OnMouseUp(drawingCanvas, e);

      if (obj != null)
      {
        Globals.Manager.SetLine((float)obj.Start.X, (float)obj.Start.Y, (float)obj.End.X, (float)obj.End.Y);
        Globals.MeshP.UpdateImage();

        obj = null;
      }

      return true;
    }
  }
}
