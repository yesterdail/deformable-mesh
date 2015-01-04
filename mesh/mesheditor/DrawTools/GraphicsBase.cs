﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Diagnostics;
using System.Globalization;


namespace mesheditor.DrawTools
{
  /// <summary>
  /// Base class for all graphics objects.
  /// </summary>
  public abstract class GraphicsBase : DrawingVisual
  {
    #region Class Members

    protected double graphicsLineWidth;
    protected Color graphicsObjectColor;

    protected bool selected;

    // Allows to write Undo - Redo functions and don't care about
    // objects order in the list.
    int objectId;

    protected const double HitTestWidth = 8.0;

    protected const double HandleSize = 12.0;

    // Objects used to draw handles. I want to keep them all time program runs,
    // therefore I don't use pens, because actual pen size can change.

    // external rectangle
    static SolidColorBrush handleBrush1 = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
    // middle
    static SolidColorBrush handleBrush2 = new SolidColorBrush(Color.FromArgb(255, 255, 255, 255));
    // internal
    static SolidColorBrush handleBrush3 = new SolidColorBrush(Color.FromArgb(255, 0, 0, 255));

    #endregion Class Members

    #region Constructor

    protected GraphicsBase()
    {
      objectId = this.GetHashCode();
    }

    #endregion Constructor

    #region Properties

    public bool IsSelected
    {
      get
      {
        return selected;
      }
      set
      {
        selected = value;

        RefreshDrawing();
      }
    }

    public double LineWidth
    {
      get
      {
        return graphicsLineWidth;
      }

      set
      {
        graphicsLineWidth = value;

        RefreshDrawing();
      }
    }

    public Color ObjectColor
    {
      get
      {
        return graphicsObjectColor;
      }

      set
      {
        graphicsObjectColor = value;

        RefreshDrawing();
      }
    }

    protected double LineHitTestWidth
    {
      get
      {
        // Ensure that hit test area is not too narrow
        return Math.Max(8.0, LineWidth);
      }
    }

    /// <summary>
    /// Object ID
    /// </summary>
    public int Id
    {
      get { return objectId; }
      set { objectId = value; }
    }


    #endregion Properties

    #region Abstract Methods and Properties

    /// <summary>
    /// Returns number of handles
    /// </summary>
    public abstract int HandleCount
    {
      get;
    }


    /// <summary>
    /// Hit test, should be overwritten in derived classes.
    /// </summary>
    public abstract bool Contains(Point point);

    /// <summary>
    /// Get handle point by 1-based number
    /// </summary>
    public abstract Point GetHandle(int handleNumber);

    /// <summary>
    /// Hit test.
    /// Return value: -1 - no hit
    ///                0 - hit anywhere
    ///                > 1 - handle number
    /// </summary>
    public abstract int MakeHitTest(Point point);


    /// <summary>
    /// Test whether object intersects with rectangle
    /// </summary>
    public abstract bool IntersectsWith(Rect rectangle);

    /// <summary>
    /// Move object
    /// </summary>
    public abstract void Move(double deltaX, double deltaY);


    /// <summary>
    /// Move handle to the point
    /// </summary>
    public abstract void MoveHandleTo(Point point, int handleNumber);

    /// <summary>
    /// Get cursor for the handle
    /// </summary>
    public abstract Cursor GetHandleCursor(int handleNumber);

    #endregion Abstract Methods and Properties

    #region Virtual Methods

    /// <summary>
    /// Normalize object.
    /// Call this function in the end of object resizing,
    /// </summary>
    public virtual void Normalize()
    {
      // Empty implementation is OK for classes which don't require
      // normalization, like line.
      // Normalization is required for rectangle-based classes.
    }

    /// <summary>
    /// Implements actual drawing code.
    /// 
    /// Call GraphicsBase.Draw in the end of every derived class Draw 
    /// function to draw tracker if necessary.
    /// </summary>
    public virtual void Draw(DrawingContext drawingContext)
    {
      if (IsSelected)
      {
        DrawTracker(drawingContext);
      }
    }


    /// <summary>
    /// Draw tracker for selected object.
    /// </summary>
    public virtual void DrawTracker(DrawingContext drawingContext)
    {
      for (int i = 1; i <= HandleCount; i++)
      {
        DrawTrackerRectangle(drawingContext, GetHandleRectangle(i));
      }
    }

    #endregion Virtual Methods

    #region Other Methods

    /// <summary>
    /// Draw tracker rectangle
    /// </summary>
    static void DrawTrackerRectangle(DrawingContext drawingContext, Rect rectangle)
    {
      // External rectangle
      drawingContext.DrawRectangle(handleBrush1, null, rectangle);

      // Middle
      drawingContext.DrawRectangle(handleBrush2, null,
          new Rect(rectangle.Left + rectangle.Width / 8,
                   rectangle.Top + rectangle.Height / 8,
                   rectangle.Width * 6 / 8,
                   rectangle.Height * 6 / 8));

      // Internal
      drawingContext.DrawRectangle(handleBrush3, null,
          new Rect(rectangle.Left + rectangle.Width / 4,
           rectangle.Top + rectangle.Height / 4,
           rectangle.Width / 2,
           rectangle.Height / 2));
    }


    /// <summary>
    /// Refresh drawing.
    /// Called after change if any object property.
    /// </summary>
    public void RefreshDrawing()
    {
      DrawingContext dc = this.RenderOpen();

      Draw(dc);

      dc.Close();
    }

    /// <summary>
    /// Get handle rectangle by 1-based number
    /// </summary>
    public Rect GetHandleRectangle(int handleNumber)
    {
      Point point = GetHandle(handleNumber);

      // Handle rectangle should have constant size, except of the case
      // when line is too width.
      double size = Math.Max(HandleSize, LineWidth * 1.1);

      return new Rect(point.X - size / 2, point.Y - size / 2,
          size, size);
    }

    #endregion Other Methods
  }
}
