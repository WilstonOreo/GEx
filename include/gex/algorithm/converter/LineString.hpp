#pragma once

#include "Generic.hpp"
#include "gex/algorithm/for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      /// Converts a line string to points
      template<typename POINT>
      struct Converter<prim::LineString<POINT>,prim::MultiPoint<POINT>>
      {
        typedef prim::LineString<POINT> linestring_type;
        typedef std::vector<typename linestring_type::point_type> points_type;

        void operator()(const linestring_type& _in, points_type& _out)
        {
          _out.clear();
          for_each_point(_in,[&](const typename linestring_type::point_type& p)
          {
          _out.push_back(p);
          });
        }
      };

      /// Convert a line string to a ring
      template<typename POINT>
      struct Converter<prim::LineString<POINT>,prim::Ring<POINT>>
      {
        typedef prim::LineString<POINT> linestring_type;
        typedef prim::Ring<POINT> ring_type;

        void operator()(const linestring_type& _in, ring_type& _out)
        {
          _out = ring_type(_in.begin(),_in.end());
        }
      };

      /// Convert a linestring into a vector of segments
      template<typename PRIMITIVE, typename POINT>
      struct Converter<PRIMITIVE,prim::MultiSegment<POINT>>
      {
        typedef prim::MultiSegment<POINT> segments_type;

        void operator()(const PRIMITIVE& _in, segments_type& _out)
        {
          for_each<prim::Segment<POINT>>(_in,[&](const prim::Segment<POINT>& _segment)
          {
            _out.push_back(_segment);
          });
        }
      };

      /// Convert a line string into a multipolygon
      template<typename MODEL>
      struct Converter<prim::LineString<MODEL>,prim::MultiPolygon<MODEL>>
      {
        typedef prim::LineString<MODEL> linestring_type;
        typedef prim::MultiPolygon<MODEL> multipolygon_type;

        void operator()(const linestring_type& _in, multipolygon_type& _out)
        {
          _out = convert<prim::MultiPolygon<MODEL>>(convert<prim::Ring<MODEL>>(_in));
        }
      };
    }
  }
}
