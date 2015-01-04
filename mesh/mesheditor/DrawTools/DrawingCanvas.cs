using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace mesheditor.DrawTools
{

  /// <summary>
  /// Tool Type
  /// </summary>
  public enum ToolType
  {
    Pointer,
    Anchor,
    Control,
    Max
  }

  /// <summary>
  /// Canvas used as host for DrawingVisual objects.
  /// Allows to draw graphics objects using mouse.
  /// </summary>
  public class DrawingCanvas : Canvas
  {
    #region Class Members

    // Collection contains instances of GraphicsBase-derived classes.
    private VisualCollection graphicsList;

    // Dependency properties
    private static readonly DependencyProperty LineWidthProperty;
    private static readonly DependencyProperty ObjectColorProperty;

    private ToolObject[] tools;
    public  ToolType toolType;

    #endregion Class Members

    #region Constructors

    public DrawingCanvas()
      :base()
    {
      graphicsList = new VisualCollection(this);

      tools = new ToolObject[(int)ToolType.Max];
      tools[(int)ToolType.Pointer] = new ToolPointer();
      tools[(int)ToolType.Anchor] = new ToolRectangleAnchor();
      tools[(int)ToolType.Control] = new ToolRectangleControl();

      //this.MouseDown += new MouseButtonEventHandler(DrawingCanvas_MouseDown);
      //this.MouseMove += new MouseEventHandler(DrawingCanvas_MouseMove);
      //this.MouseUp += new MouseButtonEventHandler(DrawingCanvas_MouseUp);
      this.Loaded += new RoutedEventHandler(DrawingCanvas_Loaded);
      this.KeyDown += new KeyEventHandler(DrawingCanvas_KeyDown);
    }

    static DrawingCanvas()
    {
      PropertyMetadata metaData;

      // LineWidth
      metaData = new PropertyMetadata(
          2.0,
          new PropertyChangedCallback(LineWidthChanged));

      LineWidthProperty = DependencyProperty.Register(
          "LineWidth", typeof(double), typeof(DrawingCanvas),
          metaData);

      // ObjectColor
      metaData = new PropertyMetadata(
          ColorConverter.ConvertFromString("#0079cb"),
          new PropertyChangedCallback(ObjectColorChanged));

      ObjectColorProperty = DependencyProperty.Register(
          "ObjectColor", typeof(Color), typeof(DrawingCanvas),
          metaData);
    }

    #endregion

    #region Dependency Properties

    #region LineWidth

    /// <summary>
    /// Line width of new graphics object.
    /// Setting this property is also applied to current selection.
    /// </summary>
    public double LineWidth
    {
      get
      {
        return (double)GetValue(LineWidthProperty);
      }
      set
      {
        SetValue(LineWidthProperty, value);

      }
    }

    /// <summary>
    /// Callback function called when LineWidth dependency property is changed
    /// </summary>
    static void LineWidthChanged(DependencyObject property, DependencyPropertyChangedEventArgs args)
    {
      DrawingCanvas d = property as DrawingCanvas;

      HelperFunctions.ApplyLineWidth(d, d.LineWidth, true);
    }

    #endregion LineWidth

    #region ObjectColor

    /// <summary>
    /// Color of new graphics object.
    /// Setting this property is also applied to current selection.
    /// </summary>
    public Color ObjectColor
    {
      get
      {
        return (Color)GetValue(ObjectColorProperty);
      }
      set
      {
        SetValue(ObjectColorProperty, value);

      }
    }

    /// <summary>
    /// Callback function called when ObjectColor dependency property is changed
    /// </summary>
    static void ObjectColorChanged(DependencyObject property, DependencyPropertyChangedEventArgs args)
    {
      DrawingCanvas d = property as DrawingCanvas;

      HelperFunctions.ApplyColor(d, d.ObjectColor, true);
    }

    #endregion ObjectColor

    #endregion Dependency Properties

    #region Internal Properties

    /// <summary>
    /// Get graphic object by index
    /// </summary>
    internal GraphicsBase this[int index]
    {
      get
      {
        if (index >= 0 && index < Count)
        {
          return (GraphicsBase)graphicsList[index];
        }

        return null;
      }
    }

    /// <summary>
    /// Get number of graphic objects
    /// </summary>
    internal int Count
    {
      get
      {
        return graphicsList.Count;
      }
    }

    /// <summary>
    /// Get number of selected graphic objects
    /// </summary>
    internal int SelectionCount
    {
      get
      {
        int n = 0;

        foreach (GraphicsBase g in this.graphicsList)
        {
          if (g.IsSelected)
          {
            n++;
          }
        }

        return n;
      }
    }

    /// <summary>
    /// Return list of graphics
    /// </summary>
    internal VisualCollection GraphicsList
    {
      get
      {
        return graphicsList;
      }
    }

    /// <summary>
    /// Returns INumerable which may be used for enumeration
    /// of selected objects.
    /// </summary>
    internal IEnumerable<GraphicsBase> Selection
    {
      get
      {
        foreach (GraphicsBase o in graphicsList)
        {
          if (o.IsSelected)
          {
            yield return o;
          }
        }
      }

    }

    #endregion Internal Properties

    #region Visual Children Overrides

    /// <summary>
    /// Get number of children: VisualCollection count.
    /// </summary>
    protected override int VisualChildrenCount
    {
      get
      {
        int n = graphicsList.Count;
        return n;
      }
    }

    /// <summary>
    /// Get visual child - one of GraphicsBase objects
    /// </summary>
    protected override Visual GetVisualChild(int index)
    {
      if (index < 0 || index >= graphicsList.Count)
      {
        throw new ArgumentOutOfRangeException("index");
      }

      return graphicsList[index];
    }

    #endregion Visual Children Overrides

    #region Mouse Event Handlers

    /// <summary>
    /// Mouse down.
    /// Left button down event is passed to active tool.
    /// Right button down event is handled in this class.
    /// </summary>
    public bool DrawingCanvas_MouseDown(object sender, MouseButtonEventArgs e)
    {
      if (tools[(int)toolType] == null)
      {
        return false;
      }

      this.Focus();

      if (e.ChangedButton == MouseButton.Left)
      {
        return tools[(int)toolType].OnMouseDown(this, e);
      }

      return false;
    }

    /// <summary>
    /// Mouse move.
    /// Moving without button pressed or with left button pressed
    /// is passed to active tool.
    /// </summary>
    public bool DrawingCanvas_MouseMove(object sender, MouseEventArgs e)
    {
      if (tools[(int)toolType] == null)
      {
        return false;
      }

      if (e.MiddleButton == MouseButtonState.Released && e.RightButton == MouseButtonState.Released)
      {
        return tools[(int)toolType].OnMouseMove(this, e);
      }
      else
      {
        this.Cursor = HelperFunctions.DefaultCursor;
      }
      return false;
    }

    /// <summary>
    /// Mouse up event.
    /// Left button up event is passed to active tool.
    /// </summary>
    public bool DrawingCanvas_MouseUp(object sender, MouseButtonEventArgs e)
    {
      if (tools[(int)toolType] == null)
      {
        return false;
      }

      if (e.ChangedButton == MouseButton.Left)
      {
        return tools[(int)toolType].OnMouseUp(this, e);
      }
      return false;
    }

    #endregion Mouse Event Handlers

    #region Other Event Handlers

    /// <summary>
    /// Initialization after control is loaded
    /// </summary>
    void DrawingCanvas_Loaded(object sender, RoutedEventArgs e)
    {
      this.Focusable = true;      // to handle keyboard messages
    }

    /// <summary>
    /// Handle keyboard input
    /// </summary>
    void DrawingCanvas_KeyDown(object sender, KeyEventArgs e)
    {
      // Esc key stops currently active operation
      if (e.Key == Key.Escape)
      {
        //if (this.IsMouseCaptured)
        {
          HelperFunctions.UnselectAll(this);
        }
      }
      else if(e.Key == Key.Delete)
      {
        //if(this.IsMouseCaptured)
        {
          HelperFunctions.DeleteSelection(this);
        }
      }
    }

    #endregion

    #region Private Functions

   
    #endregion
  }
}
