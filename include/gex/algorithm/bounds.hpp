#pragma once

#include "for_each.hpp"

namespace gex
{
  namespace strategy
  {
    namespace bounds
    {
      struct Default {};
    }
  }

  namespace algorithm
  {
    GEX_FUNCTOR(Bounds,bounds)

    template<typename PRIMITIVE>
    typename Bounds<PRIMITIVE,gex::strategy::bounds::Default>::output_type bounds(const PRIMITIVE& _primitive)
    {
      return bounds(_primitive,gex::strategy::bounds::Default());
    }

    namespace functor
    {
      using namespace gex::strategy;

      template<typename PRIMITIVE> 
      struct Bounds<PRIMITIVE,bounds::Default>
      {
        typedef typename PRIMITIVE::bounds_type output_type;

        template<typename STRATEGY>
        void operator()(const PRIMITIVE& _primitive, STRATEGY, output_type& _output)
        {
          _output = _primitive.bounds();
        }
      };

      template<typename POINT>
      struct Bounds<prim::LineString<POINT>,bounds::Default>
      {
        typedef typename prim::LineString<POINT>::bounds_type output_type;

        template<typename T, typename STRATEGY>
        void operator()(const T& _primitive, STRATEGY, output_type& _output)
        {
          for_each<POINT>(_primitive,[&](const POINT& _point)
          {
            _output.extend(_point);
          });
        }
      };
      
      template<typename POINT>
      struct Bounds<prim::MultiLineString<POINT>,bounds::Default> : 
        Bounds<prim::LineString<POINT>,bounds::Default> 
      {
        typedef typename prim::MultiLineString<POINT>::bounds_type output_type;
      };
    }
  }

  using algorithm::bounds;
}
