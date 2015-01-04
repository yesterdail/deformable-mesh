using System;
using System.IO;
using System.Windows;
using System.Windows.Input;
using mesheditor.Mesh;

namespace mesheditor.DrawTools
{
  class ToolRectangleAnchor : ToolObject
  {
    GraphicsRectangle obj = null;

    public ToolRectangleAnchor()
    {
      MemoryStream stream = new MemoryStream(Properties.Resources.Rectangle);
      ToolCursor = new Cursor(stream);
    }

    /// <summary>
    /// Create new rectangle
    /// </summary>
    public override bool OnMouseDown(DrawingCanvas drawingCanvas, MouseButtonEventArgs e)
    {
      Point p = e.GetPosition(drawingCanvas);
      obj = new GraphicsRectangle(
          p.X,
          p.Y,
          p.X + 1,
          p.Y + 1,
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
        if (drawingCanvas.IsMouseCaptured)
        {
          if (drawingCanvas.Count > 0)
          {
            drawingCanvas[drawingCanvas.Count - 1].MoveHandleTo(
                e.GetPosition(drawingCanvas), 5);

            return true;
          }
        }
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
        float[] polyx = new float[4];
        float[] polyy = new float[4];
        polyx[0] = (float)obj.Left; polyy[0] = (float)obj.Top;
        polyx[1] = (float)obj.Right; polyy[1] = (float)obj.Top;
        polyx[2] = (float)obj.Right; polyy[2] = (float)obj.Bottom;
        polyx[3] = (float)obj.Left; polyy[3] = (float)obj.Bottom;
        Globals.Manager.SetAnchorPoints(polyx, polyy, 4);
        Globals.MeshP.UpdateImage();

        obj = null;

        HelperFunctions.DeleteAll(drawingCanvas);
      }

      return true;
    }
  }
}
