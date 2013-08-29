#pragma once

#include <tbd/seq_op.h>
#include "Ring.hpp"
#include "gex/algorithm/for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      /// Converts a polygon to segments
      template<typename RING, typename POINT>
      struct Converter<prim::Polygon<RING>,prim::MultiSegment<POINT>>
      {
        typedef prim::Polygon<RING> polygon_type;
        typedef prim::MultiSegment<POINT> segments_type;

        void operator()(const polygon_type& _in, segments_type& _out)
        {
          for_each<typename polygon_type::ring_type>(_in,[&](const typename polygon_type::ring_type& r)
          {
            using tbd::seq_op::operator+=;
            _out += convert<segments_type>(r);
          });
        }
      };

      /// Convert a polygon to rings
      template<typename MODEL>
      struct Converter<prim::Polygon<MODEL>,std::vector<prim::Ring<MODEL>>>
      {
        typedef prim::Polygon<MODEL> polygon_type;
        typedef std::vector<prim::Ring<MODEL>> rings_type;

        void operator()(const polygon_type& _in, rings_type& _out)
        {
          for_each_ring(_in,[&](const typename polygon_type::ring_type& r)
          {
            _out.push_back(r);
          });
        }
      };

      /// Convert a polygon to line strings
      template<typename RING, typename POINT>
      struct Converter<prim::Polygon<RING>,prim::MultiLineString<POINT>>
      {
        typedef prim::Polygon<RING> polygon_type;
        typedef prim::MultiLineString<POINT> multilinestring_type;

        void operator()(const polygon_type& _in, multilinestring_type& _out)
        {
          for_each<RING>(_in,[&](const RING& r)
          {
            _out.push_back(convert<prim::LineString<POINT>>(r));
          });
        }
      };

      /// Convert a multi polygon to rings
      template<typename MODEL>
      struct Converter<prim::MultiPolygon<MODEL>,std::vector<prim::Ring<MODEL>>>
      {
        typedef prim::MultiPolygon<MODEL> multipolygon_type;
        typedef std::vector<prim::Ring<MODEL>> rings_type;

        void operator()(const multipolygon_type& _in, rings_type& _out)
        {
          for_each_ring(_in,[&](const typename multipolygon_type::ring_type& r)
          {
            _out.push_back(r);
          });
        }
      };

      template<typename POLYGON, typename POINT>
      struct Converter<prim::MultiPolygon<POLYGON>,prim::MultiSegment<POINT>>
      {
        typedef prim::MultiPolygon<POLYGON> multipolygon_type;
        typedef prim::MultiSegment<POINT> segments_type;

        void operator()(const multipolygon_type& _in, segments_type& _out)
        {
          for_each<typename multipolygon_type::ring_type>(_in,[&](const typename multipolygon_type::ring_type& r)
          {
            using tbd::seq_op::operator+=;
            _out += convert<segments_type>(r);
          });
        }
      };

      /// Converts a polygon to points
      template<typename MODEL>
      struct Converter<prim::MultiPolygon<MODEL>,std::vector<typename prim::MultiPolygon<MODEL>::point_type>>
      {
        typedef prim::MultiPolygon<MODEL> multipolygon_type;
        typedef std::vector<typename multipolygon_type::point_type> points_type;
        
        void operator()(const multipolygon_type& _in, points_type& _out)
        {
          for_each_ring(_in,[&](const typename multipolygon_type::ring_type& r)
          {
            using tbd::seq_op::operator+=;
            _out += convert<points_type>(r);
          });
        }
      };

      /// Converts a polygon to a multilinestring
      template<typename MODEL>
      struct Converter<prim::MultiPolygon<MODEL>,prim::MultiLineString<MODEL>>
      {
        typedef prim::MultiPolygon<MODEL> multipolygon_type;
        typedef prim::MultiLineString<MODEL> multilinestring_type;

        void operator()(const multipolygon_type& _in, multilinestring_type& _out)
        {
          for (auto& _p : _in)
          {
            using tbd::seq_op::operator+=;
            _out += convert<multilinestring_type>(_p);
          }
        }
      };
    }
  }
}

