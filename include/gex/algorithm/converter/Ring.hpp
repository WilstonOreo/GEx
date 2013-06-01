#pragma once

#include "Generic.hpp"
#include "gex/prim.hpp"
#include "gex/algorithm/unify.hpp"
#include "gex/algorithm/within.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      GEX_CONVERTER(Ring,std::vector<Point2>)
      {
        _out.clear();
        boost::geometry::for_each_point(_in,[&](const Point2& p)
        {
          _out.push_back(p);
        });
      }

      GEX_CONVERTER(Ring,std::vector<Segment>)
      {
        _out.clear();
        boost::geometry::for_each_segment(_in,detail::SegmentConverter<Segment>(_out));
      }

      GEX_CONVERTER(Ring,LineString)
      {
        _out.clear();
        boost::geometry::for_each_point(_in,[&](const Point2& p)
        {
          _out.push_back(p);
        });
      }

      GEX_CONVERTER(Ring,MultiLineString)
      {
        _out = convert<MultiLineString>(convert<LineString>(_in));
      }

      /// Ring sorter converter: Converts a set of rings into a multi polygon
      GEX_CONVERTER(std::vector<Ring>,MultiPolygon)
      {
        std::vector<Ring> _boundaries, _holes;
        for (const auto& _ring : _in)
        {
          if (_ring.size() < 3) continue;
          // Separate input into boundaries and holes
          auto& _container = (_ring.location() == Ring::OUTER) ? _boundaries : _holes;
          _container.push_back(_ring);
        }

        // Sort boundaries and holes by area
        auto _byArea = [](const Ring& _lhs, const Ring& _rhs) -> bool
        { return _lhs.area() < _rhs.area(); };
        std::sort(_boundaries.begin(),_boundaries.end(),_byArea );
        std::sort(_holes.begin(),_holes.end(),_byArea);

        // Flip boundaries and holes if there are only holes or
        // if largest hole is larger than largest boundary
        // These are indicators for wrong oriented normals
        if (!_holes.empty())
        {
          if (_boundaries.empty() || _holes.back().area() > _boundaries.back().area())
            std::swap(_boundaries,_holes);
        }

        for (auto& _boundary : _boundaries)
        {
          auto _holeIt = _holes.begin();
          // Store holes temporary in another vector as
          // they need to be unified afterwards
          std::vector<Ring> _holesTmp;
          while (_holeIt != _holes.end())
          {
            if (!within(*_holeIt,_boundary)) ++_holeIt;
            else
            {
              _holesTmp.push_back(*_holeIt);
              _holeIt = _holes.erase(_holeIt);
            }
          }
          _out.push_back(Polygon(_boundary,unify(_holesTmp)));
        }
        _out = unify(_out);
      }
    }
  }
}

