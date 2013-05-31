#pragma once

#include "Generic.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      GEX_CONVERTER(LineString,std::vector<Point2>)
      {
        _out.clear();
        boost::geometry::for_each_point(_in,[&](const Point2& p)
        {
          _out.push_back(p);
        });
      }

      GEX_CONVERTER(LineString,Ring)
      {
        _out.clear();
        boost::geometry::for_each_point(_in,[&](const Point2& p)
        {
          _out.push_back(p);
        });
        _out.update();
      }

      GEX_CONVERTER(LineString,std::vector<Segment>)
      {
        _out.clear();
        boost::geometry::for_each_segment(_in,detail::SegmentConverter<Segment>(_out));
      }

      GEX_CONVERTER(LineString,Polygon)
      {
        _out = convert<Polygon>(convert<Ring>(_in));
      }

      GEX_CONVERTER(LineString,MultiPolygon)
      {
        _out = convert<MultiPolygon>(convert<Ring>(_in));
      }
    }
  }
}
