#pragma once

#include "Generic.hpp"
#include "gex/prim.hpp"
#include "gex/algorithm/unify.hpp"
#include "gex/algorithm/within.hpp"
#include "gex/algorithm/for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      /// Converts a ring to points
      template<typename POINT>
      struct Converter<prim::Ring<POINT>,std::vector<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef std::vector<POINT> points_type;
        void operator()(const ring_type& _in, points_type& _out)
        {
          _out.clear();
          for_each<POINT>(_in,[&](const POINT& p)
          {
            _out.push_back(p);
          });
        }
      };

      /// Converts a ring to a vector of segments
      template<typename POINT>
      struct Converter<prim::Ring<POINT>,prim::MultiSegment<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef prim::MultiSegment<POINT> segments_type;

        void operator()(const ring_type& _in, segments_type& _out)
        {
          _out.clear();
          boost::geometry::for_each_segment(_in,detail::SegmentConverter<Segment>(_out));
        }
      };

      /// Converts a ring into a linestring
      template<typename POINT>
      struct Converter<prim::Ring<POINT>,prim::LineString<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef prim::LineString<POINT> linestring_type;

        void operator()(const ring_type& _in, linestring_type& _out)
        {
          _out.clear();
          for_each<POINT>(_in,[&](const POINT& p)
          {
            _out.push_back(p);
          });
        }
      };

      /// Converts a ring into a multilinestring
      template<typename POINT>
      struct Converter<prim::Ring<POINT>,prim::MultiLineString<prim::LineString<POINT>>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef prim::MultiLineString<prim::LineString<POINT>> multilinestring_type;
        void operator()(const ring_type& _in, multilinestring_type& _out)
        {
          _out = convert<MultiLineString>(convert<LineString>(_in));
        }
      };

      /// Ring sorter converter: Converts a set of rings into a multi polygon
      template<typename POINT, typename POLYGON>
      struct Converter<prim::MultiRing<POINT>,prim::MultiPolygon<POLYGON>>
      {
        typedef prim::MultiRing<POINT> rings_type;
        typedef prim::MultiPolygon<POLYGON> multipolygon_type;

        void operator()(const rings_type& _in, multipolygon_type& _out)
        {
          rings_type _boundaries, _holes;
          for (auto& _ring : _in)
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
            // Store holes temporary in another vector since
            // they need to be unified afterwards
            rings_type _holesTmp;
            while (_holeIt != _holes.end())
            {
              if (!within(*_holeIt,_boundary)) ++_holeIt;
              else
              {
                _holesTmp.push_back(*_holeIt);
                _holeIt = _holes.erase(_holeIt);
              }
            }
            _out.emplace_back(_boundary,unify(_holesTmp));
          }
          _out = unify(_out);
        }
      };
    }
  }
}

