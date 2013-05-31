#pragma once
#include "gex/prim/Vertex.hpp"
#include "gex/prim/Segment.hpp"

namespace gex
{
  namespace bsp
  {
    namespace split
    {
      template<typename PRIMITIVE>
      struct Cost
      {
        typedef PRIMITIVE primitive_type;
        typedef typename primitive_type::scalar_type scalar_type;

        scalar_type operator()(const primitive_type& _primitive)
        {
          return 1.0;
        }
      };
    }
  }
}
