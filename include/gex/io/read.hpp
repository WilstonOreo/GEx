#pragma once

#include "gex/comp.hpp"
#include "STL.hpp"

namespace gex
{
  namespace io
  {
    /// Read a mesh from file
    bool read(const std::string& _path, TriangleMesh& _mesh)
    {
      if (!STL()(_path,_mesh.triangles())) return false;
      _mesh.update();
      return true;
    }
  }
}
