#pragma once

#include "ForEach.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
     /////////////////////////////////////////////////////////////
      // ForEach Ring functor
      template<typename RING, bool IS_CONST>
      struct ForEach<RING,prim::Polygon<RING>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(RING,prim::Polygon<RING>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _polygon, FUNCTOR f)
        {
          f(_polygon.boundary());
          for (auto& _hole : _polygon.holes()) f(_hole);
        }
      };

      template<typename POLYGON, bool IS_CONST>
      struct ForEach<typename POLYGON::ring_type,prim::MultiPolygon<POLYGON>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(typename POLYGON::ring_type,prim::MultiPolygon<POLYGON>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiPolygon, FUNCTOR f)
        {
          for (auto& _polygon : _multiPolygon)
            ForEach<sub_primitive_type,POLYGON,IS_CONST>()(_polygon,f);
        } 
      };
    }
  }
}
