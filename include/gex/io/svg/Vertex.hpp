#pragma once

#include "Draw.hpp"
#include "gex/prim/Vertex.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename SCALAR>
      struct Draw<prim::Vertex<SCALAR>>
      {
        typedef prim::Vertex<SCALAR> Vertex;
        
        template<typename STYLE>
        void operator()(const Vertex& _vertex, 
                        const STYLE& _style, 
                        Buffer& _buffer)
        {
          std::stringstream ss;
          ss << "cx=\"" << _vertex.v().x() << "\" ";
          ss << "cy=\"" << _vertex.v().y() << "\" ";
          ss << "r=\"" << 5 << "\" ";
          _buffer += Shape("circle",ss.str(),_style);
        }
      };
    }
  }
}

