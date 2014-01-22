#pragma once

#include <boost/geometry/algorithms/intersects.hpp>
#include "gex/prim.hpp"
#include "for_each.hpp"

namespace gex
{
    namespace algorithm
    {
      template<typename A, typename B>
      struct Intersects
      {
        bool operator()(const A& _a, const B& _b)
        {
          return boost::geometry::intersects(_a,_b);
        }
      };

      template<typename MODEL>
      struct Intersects<
        base::Bounds<MODEL>,
        base::Bounds<MODEL>>
      {
        typedef base::Bounds<MODEL> bounds_type;
        bool operator()(const bounds_type& _a, const bounds_type& _b)
        {
          GEX_ASSERT(_a.valid() && _b.valid());
          GEX_FOREACH_DIM_(_a,i)
          {
            if (_a.min()[i] >= _b.max()[i] ||
                _a.max()[i] <= _b.min()[i])
              return false;
          }
          return true;          
        }
      };

      template<typename SCALAR>
      struct Intersects<
        base::Range<SCALAR>,
        base::Range<SCALAR>>
      {
        typedef base::Range<SCALAR> range_type;

        bool operator()(const range_type& _a, const range_type& _b)
        {
          return !(_a.min() >= _b.max() ||
                _a.max() <= _b.min());
        }
      };


      template<typename SCALAR>
      struct Intersects<
        SCALAR, base::Range<SCALAR>>
      {
        typedef SCALAR scalar_type;
        typedef base::Range<scalar_type> range_type;

        bool operator()(const scalar_type& _a, const range_type& _b)
        {
          return _a >= _b.min() && _a < _b.max();
        }
      };

      template<typename POINT>
      struct Intersects<prim::LineString<POINT>,prim::Ring<POINT>>
      {
        template<typename LINESTRING, typename RING>
        bool operator()(const LINESTRING& _lineString, const RING& _ring)
        {
          using namespace boost::geometry::model;
          linestring<POINT> _ls(_ring.begin(),_ring.end());
          return boost::geometry::intersects(_ls,_lineString);
        }
      };
      
      template<typename POINT, typename RING>
      struct Intersects<prim::LineString<POINT>,prim::Polygon<RING>>
      {
        template<typename LINESTRING, typename POLYGON>
        bool operator()(const LINESTRING& _lineString, const POLYGON& _polygon)
        {
          using namespace boost::geometry::model;
          bool _result = false;
          for_each<RING>(_polygon,[&](const RING& _ring)
          {
            if (_result) return;
            _result |= Intersects<LINESTRING,RING>()(_lineString,_ring);       
          });
          return _result;
        }
      };

      template<typename A, typename B>
      bool intersects(const A& _a, const B& _b)
      {
        return Intersects<A,B>()(_a,_b);
      }
/*
      template<typename A, typename B>
      bool operator&&(const A& _a, const B& _b)
      {
        return intersects(_a,_b);
      }*/
    }

    using algorithm::intersects;
  }
