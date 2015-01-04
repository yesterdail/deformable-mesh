using System;
using System.Windows.Input;
using System.Windows.Media;

namespace mesheditor.DrawTools
{
  /// <summary>
  /// Helper class which contains general helper functions and properties.
  /// 
  /// Most functions in this class replace VisualCollection-derived class
  /// methods, because I cannot derive from VisualCollection.
  /// They make different operations with GraphicsBase list.
  /// </summary>
  static class HelperFunctions
  {
    /// <summary>
    /// Default cursor
    /// </summary>
    public static Cursor DefaultCursor
    {
      get
      {
        return Cursors.Arrow;
      }
    }

    /// <summary>
    /// Select all graphic objects
    /// </summary>
    public static void SelectAll(DrawingCanvas drawingCanvas)
    {
      for (int i = 0; i < drawingCanvas.Count; i++)
      {
        drawingCanvas[i].IsSelected = true;
      }
    }

    /// <summary>
    /// Unselect all graphic objects
    /// </summary>
    public static void UnselectAll(DrawingCanvas drawingCanvas)
    {
      for (int i = 0; i < drawingCanvas.Count; i++)
      {
        drawingCanvas[i].IsSelected = false;
      }
    }

    /// <summary>
    /// Delete selected graphic objects
    /// </summary>
    public static void DeleteSelection(DrawingCanvas drawingCanvas)
    {
      for (int i = drawingCanvas.Count - 1; i >= 0; i--)
      {
        if (drawingCanvas[i].IsSelected)
        {
          drawingCanvas.GraphicsList.RemoveAt(i);
        }
      }
    }

    /// <summary>
    /// Delete all graphic objects
    /// </summary>
    public static void DeleteAll(DrawingCanvas drawingCanvas)
    {
      if (drawingCanvas.GraphicsList.Count > 0)
      {
        drawingCanvas.GraphicsList.Clear();
      }

    }

    /// <summary>
    /// Apply new line width
    /// </summary>
    public static bool ApplyLineWidth(DrawingCanvas drawingCanvas, double value, bool addToHistory)
    {
      bool wasChange = false;

      // LineWidth is set for all objects except of GraphicsText.
      // Though GraphicsText has this property, it should remain constant.

      foreach (GraphicsBase g in drawingCanvas.Selection)
      {
        //if (g is GraphicsLine)
        //{
        //  if (g.LineWidth != value)
        //  {
        //    g.LineWidth = value;
        //    wasChange = true;
        //  }
        //}
      }

      return wasChange;
    }

    /// <summary>
    /// Apply new color
    /// </summary>
    public static bool ApplyColor(DrawingCanvas drawingCanvas, Color value, bool addToHistory)
    {
      bool wasChange = false;

      foreach (GraphicsBase g in drawingCanvas.Selection)
      {
        if (g.ObjectColor != value)
        {
          g.ObjectColor = value;
          wasChange = true;
        }
      }

      return wasChange;
    }
  }
}
