using System;
using System.Windows;
using System.Windows.Input;

namespace mesheditor.DrawTools
{
  /// <summary>
  /// Pointer tool
  /// </summary>
  class ToolPointer : ToolObject
  {
    private enum SelectionMode
    {
      None,
      Move,           // object(s) are moved
      Size,           // object is resized
    }

    private SelectionMode selectMode = SelectionMode.None;

    // Object which is currently resized:
    private GraphicsBase selectedObject;
    private int selectedObjectHandle;

    // Keep state about last and current point (used to move and resize objects)
    private Point lastPoint = new Point(0, 0);

    public ToolPointer()
    {
    }

    /// <summary>
    /// Handle mouse down.
    /// Start moving, resizing or group selection.
    /// </summary>
    public override bool OnMouseDown(DrawingCanvas drawingCanvas, MouseButtonEventArgs e)
    {
      Point point = e.GetPosition(drawingCanvas);

      selectMode = SelectionMode.None;
      bool wasChange = false;

      GraphicsBase o;
      int handleNumber;

      // Test for resizing (only if control is selected, cursor is on the handle)
      // Test for move (cursor is on the object)
      for (int i = drawingCanvas.GraphicsList.Count - 1; i >= 0; i--)
      {
        o = drawingCanvas[i];

        //if (o.IsSelected)
        {
          handleNumber = o.MakeHitTest(point);

          if (handleNumber > 0)
          {
            selectMode = SelectionMode.Size;

            // keep resized object in class member
            selectedObject = o;
            selectedObjectHandle = handleNumber;

            HelperFunctions.UnselectAll(drawingCanvas);
            o.IsSelected = true;
            wasChange = true;
            break;
          }
          else if (handleNumber == 0)
          {
            selectMode = SelectionMode.Move;

            selectedObject = o;

            HelperFunctions.UnselectAll(drawingCanvas);
            o.IsSelected = true;

            // Set move cursor
            drawingCanvas.Cursor = Cursors.SizeAll;
            wasChange = true;
            break;
          }
        }
      }

      // Click on background
      if (selectMode == SelectionMode.None)
      {
        HelperFunctions.UnselectAll(drawingCanvas);
      }


      lastPoint = point;

      // Capture mouse until MouseUp event is received
      drawingCanvas.CaptureMouse();

      return wasChange;
    }

    /// <summary>
    /// Handle mouse move.
    /// Se cursor, move/resize, make group selection.
    /// </summary>
    public override bool OnMouseMove(DrawingCanvas drawingCanvas, MouseEventArgs e)
    {
      // Exclude all cases except left button on/off.
      if (e.MiddleButton == MouseButtonState.Pressed ||
           e.RightButton == MouseButtonState.Pressed)
      {
        drawingCanvas.Cursor = HelperFunctions.DefaultCursor;
        return false;
      }

      Point point = e.GetPosition(drawingCanvas);

      // Set cursor when left button is not pressed
      if (e.LeftButton == MouseButtonState.Released)
      {
        Cursor cursor = null;

        for (int i = 0; i < drawingCanvas.Count; i++)
        {
          int n = drawingCanvas[i].MakeHitTest(point);

          if (n > 0)
          {
            cursor = drawingCanvas[i].GetHandleCursor(n);
            break;
          }
        }

        if (cursor == null)
          cursor = HelperFunctions.DefaultCursor;

        drawingCanvas.Cursor = cursor;

        return false;

      }

      if (!drawingCanvas.IsMouseCaptured)
      {
        return false;
      }


      // Find difference between previous and current position
      double dx = point.X - lastPoint.X;
      double dy = point.Y - lastPoint.Y;

      lastPoint = point;

      // Resize
      if (selectMode == SelectionMode.Size)
      {
        if (selectedObject != null)
        {
          selectedObject.MoveHandleTo(point, selectedObjectHandle);
          return true;
        }
      }

      // Move
      if (selectMode == SelectionMode.Move)
      {
        selectedObject.Move(dx, dy);
        return true;
      }
      return false;
    }

    /// <summary>
    /// Handle mouse up.
    /// Return to normal state.
    /// </summary>
    public override bool OnMouseUp(DrawingCanvas drawingCanvas, MouseButtonEventArgs e)
    {
      if (!drawingCanvas.IsMouseCaptured)
      {
        drawingCanvas.Cursor = HelperFunctions.DefaultCursor;
        selectMode = SelectionMode.None;
        return false;
      }

      if (selectedObject != null)
      {
        // after resizing
        selectedObject.Normalize();

        selectedObject = null;
      }

      drawingCanvas.ReleaseMouseCapture();

      drawingCanvas.Cursor = HelperFunctions.DefaultCursor;

      selectMode = SelectionMode.None;

      return true;
    }

    /// <summary>
    /// Set cursor
    /// </summary>
    public override void SetCursor(DrawingCanvas drawingCanvas)
    {
      drawingCanvas.Cursor = HelperFunctions.DefaultCursor;
    }
  }
}
