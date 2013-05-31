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
      struct Pos
      {
        typedef PRIMITIVE primitive_type;
        typedef typename primitive_type::scalar_type scalar_type;

        scalar_type operator()(const primitive_type& _primitive, base::Axis _axis)
        {
          return _primitive.bounds().range(_axis).middle();
        }
      };

      template<typename MODEL>
      struct Pos<prim::Segment<MODEL>>
      {
        typedef prim::Segment<MODEL> primitive_type;
        typedef typename primitive_type::scalar_type scalar_type;

        scalar_type operator()(const primitive_type& _segment, base::Axis _axis)
        {
          return _segment.p0()[_axis];
        }
      };

      template<typename MODEL>
      struct Pos<prim::Vertex<MODEL>>
      {
        typedef prim::Vertex<MODEL> primitive_type;
        typedef typename primitive_type::scalar_type scalar_type;

        scalar_type operator()(const primitive_type& _vertex, base::Axis _axis)
        {
          return _vertex.v()[_axis];
        }
      };
    }
  }
}
