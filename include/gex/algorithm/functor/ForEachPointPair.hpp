#pragma once

#include "ForEach.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename BEGIN, typename END, typename FUNCTOR>
      void for_each_point_pair_range(BEGIN _begin, END _end, FUNCTOR f) 
      {
        if (_end - _begin <= 1) return;
        auto it = _begin;
        while (it != _end-1)
        {
          auto& _p0 = *it;
          ++it;
          auto& _p1 = *it;
          f(_p0,_p1);
        }
      }

      template<typename POINT, bool IS_CONST>
      struct ForEach<std::pair<POINT,POINT>,prim::Segment<POINT>,IS_CONST>
      {
        typedef std::pair<POINT,POINT> point_pair_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(point_pair_type,prim::Segment<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _segment, FUNCTOR f)
        {
          f(_segment.p0(),_segment.p1());
        }
      };

      template<typename POINT, bool IS_CONST>
      struct ForEach<std::pair<POINT,POINT>,prim::LineString<POINT>,IS_CONST>
      {
        typedef std::pair<POINT,POINT> point_pair_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(point_pair_type,prim::LineString<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _lineString, FUNCTOR f)
        {
          for_each_point_pair_range(_lineString.begin(),_lineString.end(),f);
        }
      };

      template<typename POINT, bool IS_CONST>
      struct ForEach<std::pair<POINT,POINT>,prim::Ring<POINT>,IS_CONST>
      {
        typedef std::pair<POINT,POINT> point_pair_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(point_pair_type,prim::Ring<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _ring, FUNCTOR f)
        {
          for_each_point_pair_range(_ring.begin(),_ring.end(),f);
        }
      };
    }
  }
}

