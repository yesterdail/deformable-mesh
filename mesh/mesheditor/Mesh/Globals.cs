using System;
using hj;

namespace mesheditor.Mesh
{
  public static class Globals
  {
    static ManagerCLR manager = null;

    static bool initialized = false;

    public static ManagerCLR Manager
    {
      set { manager = value; }
      get { return manager; }
    }

    public static bool Initialized
    {
      set { initialized = value; }
      get { return initialized; }
    }
  }
}
