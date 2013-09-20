#pragma once

#include <fstream>
#include <boost/geometry/io/wkt/read.hpp>
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

    /// Read WKT from a file
    template<typename PATH, typename PRIMITIVES>
    void readWkt(const PATH& _path, PRIMITIVES& _primitives)
    {
      std::ifstream _if(_path);
      std::string _line;
      typedef typename PRIMITIVES::value_type primitive_type;
      while(std::getline(_if, _line))
      {
        primitive_type _prim;

        boost::geometry::read_wkt(_line,_prim);
        _primitives.push_back(_prim);
      }
    }
  }
}
