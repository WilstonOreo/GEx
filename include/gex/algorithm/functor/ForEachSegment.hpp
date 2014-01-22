#pragma once

#include "ForEach.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT, typename BEGIN, typename END, typename FUNCTOR> 
      void for_each_segment_range(BEGIN _begin, END _end, FUNCTOR f)
      {
        if (_end <= _begin) return;
        auto it = _begin;
        while (it != _end-1)
        {
          auto _p0 = *it;
          ++it;
          auto _p1 = *it;
          f(prim::Segment<POINT>(_p0,_p1));
        }  
      }

      /// For Each Segment in Ring
      template<typename POINT, bool IS_CONST>
      struct ForEach<prim::Segment<POINT>,prim::Ring<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(prim::Segment<POINT>,prim::Ring<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _ring, FUNCTOR f)
        {
          for_each_segment_range<POINT>(_ring.begin(),_ring.end(),f);
        }
      };
      
      /// For Each Segment in LineString
      template<typename POINT, bool IS_CONST>
      struct ForEach<prim::Segment<POINT>,prim::LineString<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(prim::Segment<POINT>,prim::LineString<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _lineString, FUNCTOR f)
        {
          for_each_segment_range<POINT>(_lineString.begin(),_lineString.end(),f);
        }
      };
      
      template<typename POINT, bool IS_CONST>
      struct ForEach<prim::Segment<POINT>,prim::MultiLineString<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(prim::Segment<POINT>,prim::MultiLineString<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString)
            for_each_segment_range<POINT>(_lineString.begin(),_lineString.end(),f);
        }
      };
    }
  }
}
