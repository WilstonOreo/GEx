#pragma once

#include "IntersectResult.hpp"
#include "gex/prim/Segment.hpp"
#include "gex/prim/Vertex.hpp"
#include "gex/prim/LineString.hpp"

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      template<typename PRIMITIVE>
      struct Intersection
      {
        template<typename NODE_GEOMETRY>
        IntersectResult operator()(
          const PRIMITIVE& _primitive,
          const NODE_GEOMETRY& _n)
        {
          const auto& _range = _primitive.bounds().range(_n.axis());
          return IntersectResult(_range.max() <= _n.splitPos(),
                                 _range.min() >= _n.splitPos());
        }
      };

      template<typename MODEL>
      struct Intersection<base::Point<MODEL>>
      {
        template<typename NODE_GEOMETRY>
        IntersectResult operator()(
          const base::Point<MODEL>& _p,
          const NODE_GEOMETRY& _n)
        {
          return IntersectResult(_p[_n.axis()] <= _n.splitPos(),
                                 _p[_n.axis()] >= _n.splitPos());
        }
      };

      template<typename MODEL>
      struct Intersection<prim::Vertex<MODEL>>
      {
        template<typename NODE_GEOMETRY>
        IntersectResult operator()(
          const prim::Vertex<MODEL>& _v,
          const NODE_GEOMETRY& _n)
        {
          return Intersection<typename prim::Vertex<MODEL>::point_type>()(_v.v(),_n);
        }
      };

      template<typename MODEL>
      struct Intersection<prim::Segment<MODEL>>
      {
        template<typename NODE_GEOMETRY>
        IntersectResult operator()(
          const prim::Segment<MODEL>& _s,
          const NODE_GEOMETRY& _n)
        {
          return Intersection<typename prim::Segment<MODEL>::point_type>()(_s[0],_n);
        }
      };

      template<typename MODEL>
      struct Intersection<prim::LineString<MODEL>>
      {
        template<typename NODE_GEOMETRY>
        IntersectResult operator()(
          const prim::LineString<MODEL>& _l,
          const NODE_GEOMETRY& _n)
        {
          return Intersection<typename prim::LineString<MODEL>::point_type>()(_l.front(),_n);
        }
      };
    }
  }
}
