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
      template<typename MODEL>
      struct Converter<prim::Ring<MODEL>,std::vector<typename prim::Ring<MODEL>::point_type>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef std::vector<typename ring_type::point_type> points_type;
        void operator()(const ring_type& _in, points_type& _out)
        {
          _out.clear();
          for_each_point(_in,[&](const Point2& p)
          {
            _out.push_back(p);
          });
        }
      };

      /// Converts a ring to a vector of segments
      template<typename MODEL>
      struct Converter<prim::Ring<MODEL>,std::vector<typename prim::Segment<MODEL>>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef std::vector<prim::Segment<MODEL>> segments_type;

        void operator()(const ring_type& _in, segments_type& _out)
        {
          _out.clear();
          boost::geometry::for_each_segment(_in,detail::SegmentConverter<Segment>(_out));
        }
      };

      /// Converts a ring into a linestring
      template<typename MODEL>
      struct Converter<prim::Ring<MODEL>,prim::LineString<MODEL>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef prim::LineString<MODEL> linestring_type;

        void operator()(const ring_type& _in, linestring_type& _out)
        {
          _out.clear();
          for_each_point(_in,[&](const Point2& p)
          {
            _out.push_back(p);
          });
        }
      };

      /// Converts a ring into a multilinestring
      template<typename MODEL>
      struct Converter<prim::Ring<MODEL>,prim::MultiLineString<MODEL>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef prim::MultiLineString<MODEL> multilinestring_type;
        void operator()(const ring_type& _in, multilinestring_type& _out)
        {
          _out = convert<MultiLineString>(convert<LineString>(_in));
        }
      };

      /// Ring sorter converter: Converts a set of rings into a multi polygon
      template<typename MODEL>
      struct Converter<std::vector<prim::Ring<MODEL>>,prim::MultiPolygon<MODEL>>
      {
        typedef std::vector<prim::Ring<MODEL>> rings_type;
        typedef prim::MultiPolygon<MODEL> multipolygon_type;

        void operator()(const rings_type& _in, multipolygon_type& _out)
        {
          std::vector<Ring> _boundaries, _holes;
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
      };
    }
  }
}

