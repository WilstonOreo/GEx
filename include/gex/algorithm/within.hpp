#pragma once

#include "boost.hpp"
#include "convert.hpp"
#include "for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename A, typename B>
      struct Within
      {
        bool operator()(const A& _a, const B& _b)
        {
          return boost::geometry::within(_a,_b);
        }
      };
    }

    using functor::Within;

    template<typename A, typename B>
    bool within(const A& _a, const B& _b)
    {
      return Within<A,B>()(_a,_b);
    }

    namespace functor
    {
      template<class MODEL>
      struct Within<base::Point<MODEL>,base::Bounds<MODEL>>
      {
        typedef base::Point<MODEL> point_type;
        typedef base::Bounds<MODEL> bounds_type;

        bool operator()(const point_type& _a, const bounds_type& _b)
        {
          GEX_ASSERT(_b.valid());
          GEX_FOREACH_DIM_(_a,i)
          {
            if (_a[i] < _b.min()[i] || _a[i] > _b.max()[i])
              return false;
          }
          return true;         
        }
        
      };

      template<typename MODEL>
      struct Within<base::Bounds<MODEL>,base::Bounds<MODEL>>
      {
        typedef base::Bounds<MODEL> bounds_type;
        bool operator()(const bounds_type& _a, const bounds_type& _b)
        {
          GEX_FOREACH_DIM_(_a,i)
          {
            if (_a.min()[i] < _b.min()[i] ||
                _a.max()[i] > _b.max()[i]) return false;
          }
          return true;
        }
      };


      template<typename POINT>
      struct Within<prim::Segment<POINT>,prim::Ring<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef prim::Segment<POINT> segment_type;
        bool operator()(const segment_type& _seg, const ring_type& _ring)
        {
          if (!within(_seg.bounds(),_ring.bounds())) return false;
          if (!within(_seg.front(), _ring) || !within(_seg.back(),_ring)) return false;

          using namespace boost::geometry::model;
          auto&& _ls = converter::convert<prim::LineString<POINT>>(_seg);
          return boost::geometry::disjoint(_ls,
                                           linestring<POINT>(_ring.begin(), _ring.end()));
        }
      };
      
      template<typename POINT>
      struct Within<prim::Ring<POINT>,prim::Ring<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        bool operator()(const ring_type& _a, const ring_type& _b)
        {
          if (!within(_a.bounds(),_b.bounds())) return false;
          typedef typename ring_type::point_type point_type;
          if (!within(_a.front(), _b)) return false;

          using namespace boost::geometry::model;
          return boost::geometry::disjoint(linestring<point_type>(_a.begin(), _a.end()),
                                           linestring<point_type>(_b.begin(), _b.end()));
        }
      };

      template<typename POINT, typename RING>
      struct Within<prim::Segment<POINT>,prim::Polygon<RING>>
      {
        typedef prim::Polygon<RING> polygon_type;
        typedef RING ring_type;
        typedef prim::Segment<POINT> segment_type;
        bool operator()(const segment_type& _seg, const polygon_type& _polygon)
        {
          if (!within(_seg.bounds(),_polygon.bounds())) return false;
          if (!within(_seg.front(), _polygon) || !within(_seg.back(),_polygon)) return false;

          using namespace boost::geometry::model;
          auto&& _ls = converter::convert<prim::LineString<POINT>>(_seg);  
          bool _found = false;
          for_each<RING>(_polygon,[&](const RING& _ring)
          {
            if (_found) return;
            _found |= boost::geometry::disjoint(_ls,
                                           linestring<POINT>(_ring.begin(), _ring.end()));
          });
          return _found;
        }
      };
    }
  }

  using algorithm::within;
}
