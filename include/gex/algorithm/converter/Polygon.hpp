#pragma once

#include "Ring.hpp"
#include "gex/algorithm/for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      GEX_CONVERTER(Polygon,std::vector<Point2>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          std::vector<Point2>&& _points = convert<std::vector<Point2>>(r);
          _out.insert(_out.end(),_points.begin(),_points.end());
        });
      }

      GEX_CONVERTER(Polygon,std::vector<Segment>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          std::vector<Segment>&& _ringSegments = convert<std::vector<Segment>>(r);
          _out.insert(_out.end(),_ringSegments.begin(),_ringSegments.end());
        });
      }

      GEX_CONVERTER(Polygon,std::vector<Ring>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          _out.push_back(r);
        });
      }

      GEX_CONVERTER(Polygon,MultiLineString)
      {
        _out.clear();
        for_each_ring(_in,[&](const Ring& r)
        {
          _out.push_back(convert<LineString>(r));
        });
      }

      GEX_CONVERTER(MultiPolygon,std::vector<Ring>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          _out.push_back(r);
        });
      }

      GEX_CONVERTER(MultiPolygon,std::vector<Segment>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          std::vector<Segment>&& _ringSegments = convert<std::vector<Segment>>(r);
          _out.insert(_out.end(),_ringSegments.begin(),_ringSegments.end());
        });
      }

      GEX_CONVERTER(MultiPolygon,std::vector<Point2>)
      {
        for_each_ring(_in,[&](const Ring& r)
        {
          std::vector<Point2>&& _points = convert<std::vector<Point2>>(r);
          _out.insert(_out.end(),_points.begin(),_points.end());
        });
      }

      GEX_CONVERTER(MultiPolygon,MultiLineString)
      {
for (auto& _p : _in)
        {
          auto&& _multiLineString = convert<MultiLineString>(_p);
          _out.insert(_out.end(),_multiLineString.begin(),_multiLineString.end());
        }
      }
    }
  }
}

