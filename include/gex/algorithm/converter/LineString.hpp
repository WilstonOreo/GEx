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
      template<typename MODEL>
      struct Converter<prim::LineString<MODEL>,std::vector<typename prim::LineString<MODEL>::point_type>>
      {
        typedef prim::LineString<MODEL> linestring_type;
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
      template<typename MODEL>
      struct Converter<prim::LineString<MODEL>,prim::Ring<MODEL>>
      {
        typedef prim::LineString<MODEL> linestring_type;
        typedef prim::Ring<MODEL> ring_type;

        void operator()(const linestring_type& _in, ring_type& _out)
        {
          _out = ring_type(_in.begin(),_in.end());
        }
      };

      /// Convert a linestring into a vector of segments
      template<typename MODEL>
      struct Converter<prim::LineString<MODEL>,std::vector<prim::Segment<MODEL>>>
      {
        typedef prim::LineString<MODEL> linestring_type;
        typedef std::vector<prim::Segment<MODEL>> segments_type;

        void operator()(const linestring_type& _in, segments_type& _out)
        {
          _out.clear();
          boost::geometry::for_each_segment(_in,
            detail::SegmentConverter<prim::Segment<MODEL>>(_out));
        }
      };

      /// Convert a line string into a polygon 
      template<typename MODEL>
      struct Converter<prim::LineString<MODEL>,prim::Polygon<MODEL>>
      {
        typedef prim::LineString<MODEL> linestring_type;
        typedef prim::Polygon<MODEL> polygon_type;

        void operator()(const linestring_type& _in, polygon_type& _out)
        {
          _out = convert<prim::Polygon<MODEL>>(convert<prim::Ring<MODEL>>(_in));
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
