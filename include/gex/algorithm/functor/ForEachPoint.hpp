#pragma once

#include "ForEach.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace 
      {
        template<typename BEGIN, typename END, typename FUNCTOR> 
        void for_each_point_range(BEGIN _begin, END _end, FUNCTOR f)
        {
          for (auto it = _begin; it != _end; ++it)
            f(*it);
        }
      }

#define GEX_ALGORITHM_FOREACH_POINT(primitive_type)\
      template<typename POINT, bool IS_CONST>\
      struct ForEach<POINT,primitive_type<POINT>,IS_CONST>\
      {\
        GEX_ALGORITHM_FOREACH_TYPEDEFS(POINT,primitive_type<POINT>)\
        template<typename FUNCTOR>\
        void operator()(prim_ref_type _prim, FUNCTOR f);\
      };\
      template<typename POINT, bool IS_CONST> template<typename FUNCTOR>\
      void ForEach<POINT,primitive_type<POINT>,IS_CONST>::operator()(prim_ref_type _prim, FUNCTOR f)

      GEX_ALGORITHM_FOREACH_POINT(prim::Segment)
      {
        f(_prim[0]);
        f(_prim[1]);
      }

      GEX_ALGORITHM_FOREACH_POINT(prim::LineString)
      {
        for_each_point_range(_prim.begin(),_prim.end(),f);
      }

      GEX_ALGORITHM_FOREACH_POINT(prim::Ring)
      {
        for_each_point_range(_prim.begin(),_prim.end(),f);
      }

      GEX_ALGORITHM_FOREACH_POINT(std::vector)
      {
        for_each_point_range(_prim.begin(),_prim.end(),f);
      }

      template<typename POINT, bool IS_CONST>
      struct ForEach<POINT,prim::MultiLineString<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(POINT,prim::MultiLineString<POINT>)
        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString) 
            ForEach<POINT,typename prim::MultiLineString<POINT>::value_type,IS_CONST>()(_lineString,f);
        }
      };
      
      template<typename POINT, bool IS_CONST>
      struct ForEach<POINT,prim::MultiSegment<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(POINT,prim::MultiSegment<POINT>)
        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiSegment, FUNCTOR f)
        {
          for (auto& _segment : _multiSegment) 
            ForEach<POINT,typename prim::MultiSegment<POINT>::value_type,IS_CONST>()(_segment,f);
        }
      };

      template<typename RING, bool IS_CONST>
      struct ForEach<typename RING::point_type,prim::Polygon<RING>,IS_CONST>
      {
        typedef typename RING::point_type point_type;        
        GEX_ALGORITHM_FOREACH_TYPEDEFS(point_type,prim::Polygon<RING>)
        template<typename FUNCTOR>
        void operator()(prim_ref_type _polygon, FUNCTOR f)
        {
          typedef typename tbd::AddConstIf<RING,IS_CONST>::ref_type ring_ref_type;
          ForEach<RING,primitive_type,IS_CONST>()(_polygon,[&](ring_ref_type _ring)
          {
            ForEach<point_type,RING,IS_CONST>()(_ring,f);
          });
        }
      };

      template<typename POLYGON, bool IS_CONST>
      struct ForEach<typename POLYGON::point_type,prim::MultiPolygon<POLYGON>,IS_CONST>
      {
        typedef typename POLYGON::point_type point_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(point_type,prim::MultiPolygon<POLYGON>)
        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiPolygon, FUNCTOR f)
        {
          typedef typename tbd::AddConstIf<POLYGON,IS_CONST>::ref_type polygon_ref_type;
          ForEach<POLYGON,primitive_type,IS_CONST>()(_multiPolygon,[&](polygon_ref_type _polygon)
          {
            ForEach<point_type,POLYGON,IS_CONST>()(_polygon,f);
          });
        }
      };
    }
  }
}




