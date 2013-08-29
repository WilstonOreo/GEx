#pragma once

#include <tbd/add_const_if.h>
#include "gex/prim/Segment.hpp"
#include "gex/prim/LineString.hpp"
#include "gex/prim/Ring.hpp"
#include "gex/prim/Polygon.hpp"
#include "gex/prim/MultiPolygon.hpp"

#define GEX_ALGORITHM_FOREACH_TYPEDEFS(sub_primitive,primitive)\
        typedef sub_primitive sub_primitive_type;\
        typedef primitive primitive_type;\
        typedef typename tbd::AddConstIf<sub_primitive,IS_CONST>::ref_type sub_ref_type;\
        typedef typename tbd::AddConstIf<primitive,IS_CONST>::ref_type prim_ref_type;

namespace gex
{
    namespace algorithm
    {
      namespace functor
      {
      template<typename SUB_PRIMITIVE, typename PRIMITIVE, bool IS_CONST>
      struct ForEach
      {
      };

      template<typename PRIMITIVE, bool IS_CONST>
      struct ForEach<PRIMITIVE,PRIMITIVE,IS_CONST>
      {
        typedef typename tbd::AddConstIf<PRIMITIVE,IS_CONST>::ref_type prim_ref_type;
        
        template<typename FUNCTOR>
        void operator()(prim_ref_type _prim, FUNCTOR f)
        {
          f(_prim);
        }
      };

      template<typename SUB_PRIMITIVE, typename RING, bool IS_CONST>
      struct ForEach<SUB_PRIMITIVE,prim::Polygon<RING>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(SUB_PRIMITIVE,prim::Polygon<RING>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _polygon, FUNCTOR f)
        {
          typedef typename tbd::AddConstIf<RING,IS_CONST>::ref_type ring_ref_type;
          ForEach<RING,primitive_type,IS_CONST>()(_polygon,[&](ring_ref_type _ring)
          {
            ForEach<sub_primitive_type,RING,IS_CONST>()(_ring,f);
          });
        }
      };

      template<typename SUB_PRIMITIVE, typename POLYGON, bool IS_CONST>
      struct ForEach<SUB_PRIMITIVE,prim::MultiPolygon<POLYGON>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(SUB_PRIMITIVE,prim::MultiPolygon<POLYGON>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiPolygon, FUNCTOR f)
        {
          typedef typename tbd::AddConstIf<POLYGON,IS_CONST>::ref_type polygon_ref_type;
          ForEach<POLYGON,primitive_type,IS_CONST>()(_multiPolygon,[&](polygon_ref_type _polygon)
          {
            ForEach<sub_primitive_type,POLYGON,IS_CONST>()(_polygon,f);
          });
        }
      };
    }
  }
}



