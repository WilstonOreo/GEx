#pragma once

#include "for_each.hpp"
#include "distance.hpp"
#include "convert.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct ClosestPoint {};
      
      template<typename POINT>
      struct ClosestPoint<prim::Segment<POINT>>
      {
        typedef prim::Segment<POINT> primitive_type;

        void operator()(
            const primitive_type& _segment,
            const POINT& _point,
            POINT& _closestPoint)
        {
          auto& _p0 = _segment.p0();
          auto& _p1 = _segment.p1();
          auto&& _d = _p1 - _p0;
          auto _t = _d.dot(_point - _p0) / _d.squaredNorm(); 
          if (_t <= 0) { _closestPoint = _p0; return; }
          if (_t >= 1) { _closestPoint = _p1; return; }
          _closestPoint = _p0 + _t * _d;
        }
      };

      namespace 
      {
        template<typename SUBPRIMITIVE,typename PRIMITIVE, typename POINT>
        void closest(const PRIMITIVE& _prim, const POINT& _point, POINT& _closestPoint)
        {
          typedef typename POINT::Scalar scalar_type;
          typedef prim::Segment<POINT> segment_type;
          
          auto _minDist = inf<scalar_type>();
          functor::ForEach<SUBPRIMITIVE,PRIMITIVE,true>()(_prim,[&](const SUBPRIMITIVE& _segment)
          {
            POINT _subPoint;
            ClosestPoint<SUBPRIMITIVE>()(_segment,_point,_subPoint);
            auto&& _dist = sqrDistance(_point,_subPoint);
            if (_dist < _minDist)
            {
              _closestPoint = _subPoint;
              _minDist = _dist;
            }
          });
        }
      }

      template<typename POINT>
      struct ClosestPoint<prim::LineString<POINT>>
      {
        typedef prim::LineString<POINT> primitive_type;

        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Segment<POINT> segment_type;
          closest<segment_type>(_prim,_point,_closestPoint);
        }
      };

      template<typename POINT>
      struct ClosestPoint<prim::Ring<POINT>>
      {
        typedef prim::Ring<POINT> primitive_type;

        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Segment<POINT> segment_type;
          closest<segment_type>(_prim,_point,_closestPoint);
        }
      };

      template<typename RING>
      struct ClosestPoint<prim::Polygon<RING>>
      {
        typedef prim::Polygon<RING> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          closest<RING>(_prim,_point,_closestPoint);
        }
      };

      template<typename POLYGON>
      struct ClosestPoint<prim::MultiPolygon<POLYGON>>
      {
        typedef prim::MultiPolygon<POLYGON> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef typename POLYGON::ring_type ring_type;
          bool _found = true;
          typedef typename POINT::Scalar scalar_type;
          auto _minDist = inf<scalar_type>();
          for (auto& _polygon : _prim)
          {
            POINT _p;
            closest<ring_type>(_prim,_point,_p);
            auto _dist = gex::sqrDistance(_p,_closestPoint);
            if (_found || _dist < _minDist)
            {
              _minDist = _dist;
              _found = false;
              _closestPoint = _p;
            }
          }
        }
      };
    }

    using functor::ClosestPoint;

    template<typename PRIMITIVE, typename POINT> 
    void closestPoint(
        const PRIMITIVE& _prim, 
        const POINT& _point,
        POINT& _closestPoint)
    {
      ClosestPoint<PRIMITIVE>()(_prim,_point,_closestPoint);
    }

    template<typename PRIMITIVE, typename POINT> 
    POINT closestPoint(
        const PRIMITIVE& _prim, 
        const POINT& _point)
    {
      POINT _closestPoint;
      closestPoint(_prim,_point,_closestPoint);
      return _closestPoint;
    }
  }

  using algorithm::closestPoint;
}


