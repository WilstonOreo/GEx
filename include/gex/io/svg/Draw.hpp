#pragma once 

#include "Shape.hpp"
#include "gex/prim.hpp"


namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename PRIMITIVE>
      struct Draw
      {

      }; 

      template<typename PRIMITIVE>
      struct Draw<std::vector<PRIMITIVE>>
      {
        template<typename P, typename STYLE, typename BUFFER>
        void operator()(const P& _primitives, 
                        const STYLE& _style, 
                        BUFFER& _buffer)
        {
          for (auto& _prim : _primitives)
            Draw<typename std::decay<decltype(_prim)>::type>()(_prim,_style,_buffer);
        }
      };
    }
  }
}

