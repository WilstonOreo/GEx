#pragma once

#include "closestPoint.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct MarkerFromPoint {};

      template<typename POINT>
      struct MarkerFromPoint<prim::Segment<POINT>> 
      {
        typedef prim::Segment<POINT> primitive_type;
        
        template<typename MARKER>
        void operator()(const primitive_type& _segment, const POINT& _point, MARKER& _marker)
        {
          auto& _p0 = _segment.p0();
          auto& _p1 = _segment.p1();
          auto&& _closestPoint = closestPoint(_segment,_point);
          return (_p0 - _closestPoint).length() / (_p1 - _p0).length();
        }
      };

      namespace 
      {
        template<
          typename SUBPRIMITIVE,
          typename BEGIN, 
          typename END, 
          typename LENGTH, 
          typename POINT, 
          typename MARKER>
        void marker(
            BEGIN _begin, 
            END _end, 
            const LENGTH& _length, const POINT& _point ,MARKER& _marker)
        {
          MARKER _pos = 0, _minPos = 0, _minDist = misc::inf<>();
          POINT _closestPoint;
          for_each_segment_range<POINT>(_begin,_end,[&](const SUBPRIMITIVE& _segment)
          {
            auto& _p0 = _segment.p0();
            auto& _p1 = _segment.p1();
            POINT _closestPoint;
            auto&& _subPoint = closestPoint(_segment,_point);
            auto&& _dist = sqrDistance(_point,_subPoint);
            if (_dist < _minDist)
            {
              _closestPoint = _subPoint;
              _minDist = _dist;
              _minPos = _pos + (_p0 - _subPoint).length();
            }
            _pos += (_p1 - _p0).length();
          });
          _marker = _minPos / _length;
        }
      }

      template<typename POINT>
      struct MarkerFromPoint<prim::Ring<POINT>> 
      {
        typedef prim::Ring<POINT> primitive_type;

        template<typename MARKER>
        void operator()(const primitive_type& _ring, const POINT& _point, MARKER& _marker) 
        {
          typedef prim::Segment<POINT> segment_type;
          auto&& _length = _ring.length();
          marker<segment_type>(_ring.begin(),_ring.end(),_length,_point,_marker);
        }
      };

      template<typename POINT>
      struct MarkerFromPoint<prim::LineString<POINT>> 
      {
        typedef prim::LineString<POINT> primitive_type;

        template<typename MARKER>
        void operator()(const primitive_type& _lineString, const POINT& _point, MARKER& _marker) 
        {
          typedef prim::Segment<POINT> segment_type;
          auto&& _length = perimeter(_lineString);
          marker<segment_type>(_lineString.begin(),_lineString.end(),_length,_point,_marker);
        }
      };
    }

    using functor::MarkerFromPoint;

    template<typename PRIMITIVE, typename POINT, typename MARKER>
    void markerFromPoint(const PRIMITIVE& _primitive, const POINT& _point, MARKER& _marker)
    {
      MarkerFromPoint<PRIMITIVE>()(_primitive,_point,_marker);
    }
    
    template<typename PRIMITIVE, typename POINT>
    typename PRIMITIVE::scalar_type markerFromPoint(const PRIMITIVE& _primitive, const POINT& _point)
    {
      typename PRIMITIVE::scalar_type _marker;
      markerFromPoint(_primitive,_point,_marker);
      return _marker;
    }
  }
}


