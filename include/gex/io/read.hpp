#pragma once

#include "gex/comp.hpp"
#include "STL.hpp"

namespace gex
{
  namespace io
  {
    /// Read a mesh from file
    template<typename PATH, typename SCALAR>
    bool read(const PATH& _path, comp::TriangleMesh<SCALAR>& _mesh)
    {
      if (!STL()(_path,_mesh.triangles())) return false;
      _mesh.update();
      return true;
    }
  }
}
