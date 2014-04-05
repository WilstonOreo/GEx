#pragma once

#include <gex/strategy/offset.hpp>

namespace gex
{
  namespace algorithm
  {
    GEX_FUNCTOR(Offset,offset)
  }
  using algorithm::offset; 
}

#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/unify.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/extensions/algorithms/buffer/buffer_inserter.hpp>

#include <boost/geometry/extensions/strategies/buffer.hpp>
#include <boost/geometry/extensions/strategies/buffer_end_round.hpp>
#include "functor/Offset.hpp"
#include <gex/strategy/offset.hpp>
#include "medial_axis.hpp"


namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT>
      struct Offset<prim::Segment<POINT>,strategy::offset::Default>
      {
        typedef prim::Segment<POINT> segment_type;
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> output_type;

        template<typename STRATEGY>
        void operator()(const segment_type& _segment, STRATEGY _s, output_type& _out)
        {
          typedef prim::LineString<POINT> linestring_type;
          auto&& _lineString = convert<linestring_type>(_segment);
          offset(_lineString,strategy::offset::Default(_s.offset()),_out);
        }
      };
      
      template<typename POINT>
      struct Offset<prim::LineString<POINT>,strategy::offset::Default>
      {
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> output_type;
        typedef prim::LineString<POINT> linestring_type;

        template<typename STRATEGY>
        void operator()(const linestring_type& _lineString, STRATEGY _s, output_type& _out)
        {
          if (_lineString.empty()) return;
          if ((sqrDistance(_lineString.back(),_lineString.front()) == 0.0) && 
              _lineString.size() <= 2) return;  

          auto& _offset = _s.offset();

          if (_offset < 0.0) return;
          namespace bg = boost::geometry;
          namespace buf = bg::strategy::buffer;
          buf::join_round<POINT,POINT> _joinStrategy(16 + _offset * 10);
          buf::end_round<POINT,POINT> _endStrategy(16 + _offset * 10);
          buf::distance_symmetric<Scalar> _distanceStrategy(_offset);
          bg::buffer_inserter<polygon_type>(_lineString, std::back_inserter(_out),
                        _distanceStrategy, 
                        _joinStrategy,
                        _endStrategy);
        }
      };
      
      template<typename POINT>
      struct Offset<prim::MultiLineString<POINT>,strategy::offset::Default>
      {
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> output_type;
        typedef prim::LineString<POINT> linestring_type;
        typedef prim::MultiLineString<POINT> multilinestring_type;

        template<typename STRATEGY>
        void operator()(const multilinestring_type& _multiLineString, STRATEGY _s, output_type& _out)
        {
          output_type _lineStringPolygons;
          auto& _offset = _s.offset();
          for (auto& _lineString : _multiLineString)
          {
            offset(_lineString,_s,_out);
          }
        }
      };

      template<typename POINT>
      struct Offset<prim::Ring<POINT>,strategy::offset::Default>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef POINT point_type;
        typedef prim::Polygon<ring_type> polygon_type;
        typedef prim::MultiPolygon<polygon_type> multipolygon_type;
        typedef multipolygon_type output_type;
        typedef typename POINT::Scalar scalar_type;

        /// Returns true if offset variant was successful, false if boost polygon was used
        template<typename BOUNDS, typename STRATEGY, typename SCALAR>
        bool withoutFallback(const ring_type& _in, const BOUNDS& _bounds, STRATEGY _s, output_type& _output, 
                        SCALAR _limit)
        {
          typedef typename ring_type::scalar_type scalar_type;
          auto& _offset = _s.offset();

          _output.clear();
          if (std::abs(_offset) <= _limit)
          {
            _output.emplace_back(_in);
            return true;
          }

          polygon_type _offsetPolygon;

          if (detail::generateOffsetPolygon(_in,std::abs(_offset),_limit,_offsetPolygon))
          {
            if (_offset > 0)
            {
              _output.push_back(_offsetPolygon.boundary());
              for (auto& _hole : _offsetPolygon.holes())
                if (disjoint(_hole,_in))
                  _output.back().holes().emplace_back(_hole);
            }
            else
            {
                for (auto& _hole : _offsetPolygon.holes())
                  if (within(_hole,_in))
                    _output.emplace_back(_hole);
                _output.update();
            }
            return true;
          }
          return false;
        }

        template<typename STRATEGY>
        void operator()(const ring_type& _in, STRATEGY _s, output_type& _output, 
                        Scalar _eps = 0.001)
        {
          auto& _offset = _s.offset();
          typedef typename ring_type::bounds_type bounds_type;
          
          ring_type _simplified;
          auto& _bounds = _in.bounds();
          auto _limit = _bounds.size().norm() * _eps;
          boost::geometry::simplify(_in,_simplified,_limit);
          _simplified.update();

          if (!withoutFallback(_simplified,_in.bounds(),_s,_output,_limit))
          {
            /// Boost polygon minkowski sum fallback if generating offset polygon was not successful
            auto _maxExtent = std::max(_bounds.size()(X),_bounds.size()(Y));
            point_type _max(_maxExtent,_maxExtent);
            bounds_type _polyBounds(_bounds.center() - _max,_bounds.center() + _max);
            auto&& _polygons = polygon::adapt(gex::convert<gex::MultiPolygon>(_simplified),_polyBounds);
            _polygons.resize(_offset/(_maxExtent*2)*gex::polygon::size(),true,std::max(4,std::min(int(std::abs(_offset) / _limit),16)));
            _output = polygon::adapt(_polygons,_polyBounds);
          }
        }
      };     

      template<typename RING>
      struct Offset<prim::Polygon<RING>,strategy::offset::Default>
      {
        typedef prim::MultiPolygon<prim::Polygon<RING>> output_type;

        template<typename STRATEGY>
        void operator()(
          const prim::Polygon<RING>& _in, STRATEGY _s, 
          output_type& _out, 
          Scalar _eps = 0.001)
        {
          auto& _offset = _s.offset();
          typedef Offset<RING,STRATEGY> offset_type;
          if (_in.holes().empty())
          {
            offset_type()(_in.boundary(),_s,_out,_eps);
            return;
          }
          
          auto& _bounds = _in.bounds();
          Scalar _limit = _bounds.size().norm() * _eps;
          prim::Polygon<RING> _simplified;
          boost::geometry::simplify(_in,_simplified,_limit);
          _simplified.update();

          output_type _boundary, _holes;
          if (!offset_type().withoutFallback(_simplified.boundary(),_bounds,_s,_boundary,_limit))
          {
            boostPolygonFallback(_simplified,_bounds,_offset,_out,_limit);
            return;
          }
 
          for (auto& _r : _simplified.holes())
          {
            output_type _holesBuffer;
            if (!offset_type().withoutFallback(_r,_bounds,STRATEGY(-_offset),_holesBuffer,_limit))
            {
              boostPolygonFallback(_in,_bounds,_offset,_out,_limit);
              return;
            }
            _holes.insert(_holes.end(),_holesBuffer.begin(),_holesBuffer.end());
          }

          for(auto& _hole : _holes) 
          {
            _hole.update();
          }
         
          if (_offset < 0.0)
            _holes = unify(_holes);
          difference(_boundary,_holes,_out);
        }

      private:

        template<typename BOUNDS, typename OFFSET, typename SCALAR>
        void boostPolygonFallback(const prim::Polygon<RING>& _in,
            const BOUNDS& _bounds,
            const OFFSET& _offset, 
          output_type& _out, 
          SCALAR _limit)
        {
          typedef typename RING::point_type point_type;
          typedef typename RING::bounds_type bounds_type;
          
          /// Boost polygon minkowski sum fallback if generating offset polygon was not successful
          auto _maxExtent = std::max(_bounds.size()(X),_bounds.size()(Y));
          point_type _max(_maxExtent,_maxExtent);
          bounds_type _polyBounds(_bounds.center() - _max,_bounds.center() + _max);
          auto&& _polygons = polygon::adapt(gex::convert<gex::MultiPolygon>(_in),_polyBounds);
          _polygons.resize(_offset/(_maxExtent*2)*gex::polygon::size(),true,std::max(4,std::min(int(std::abs(_offset) / _limit),16)));
          _out = polygon::adapt(_polygons,_polyBounds);
        }

      };

      template<typename POLYGON>
      struct Offset<prim::MultiPolygon<POLYGON>,strategy::offset::Default>
      {
        typedef prim::MultiPolygon<POLYGON> output_type;

        template<typename STRATEGY>
        void operator()(const output_type& _in, STRATEGY _s, output_type& _out)
        {
          auto& _offset = _s.offset();
          output_type _offsetPolygons;
          for (auto& _p : _in)
          {
            output_type _tmp;
            Offset<POLYGON,STRATEGY>()(_p,_s,_tmp);
            _offsetPolygons.insert(_offsetPolygons.end(),_tmp.begin(),_tmp.end()); 
          }
          if (_offset > 0.0)
          {
            _offsetPolygons = unify(_offsetPolygons);
          } 

          _out.insert(_out.end(),_offsetPolygons.begin(),_offsetPolygons.end());
          for (auto& _p : _out)
          {
            _p.update();
          }
        }
      };

      /// NonUniform offset
      template<typename POINT>
      struct Offset<prim::Segment<POINT>,strategy::offset::NonUniform<prim::Segment<POINT>>>
      {
        typedef POINT point_type;
        typedef prim::Segment<point_type> segment_type;
        typedef std::pair<Scalar,Scalar> offset_type;
        typedef prim::Ring<point_type> output_type;
       
        template<typename STRATEGY>
        void operator()(
            const segment_type& _seg, 
            STRATEGY _s,
            output_type& _ring)
        {
          auto&& _offset = _s.offset();
          _ring.clear();
          auto _d = (_seg.p1() - _seg.p0()).normalized();
          Scalar _angle = atan2(_d.y(),_d.x());

          Scalar _pi2 = M_PI / 2.0;
          detail::buildArc(_seg.p0(),_angle+_pi2,_angle+3*_pi2,_offset.first,0.001,_ring);
          detail::buildArc(_seg.p1(),_angle-_pi2,_angle+_pi2,_offset.second,0.001,_ring);
          _ring.update();
        }
      };

      template<typename PRIMITIVE>
      struct Offset<PRIMITIVE,strategy::offset::MultiDistances>
      {
        typedef MultiPolygon output_type;

        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          PRIMITIVE _simplified;
          if (_strategy.simplify() > 0)
            gex::algorithm::simplify(_primitive,_simplified,_strategy.simplify());
          else
            _simplified = _primitive;

          typename OUTPUT::value_type _perimeters, _out, _offsetPolygons;
          for (auto& _offset : _strategy.distances())
          {
            _offsetPolygons.clear();
            offset(_simplified,strategy::offset::Default(_offset),_offsetPolygons);
            std::cout << _offset << std::endl;
            if (_offsetPolygons.empty()) break;
            
            _offsetPolygons.update();
            _output.push_back(_offsetPolygons);
          }
        }
      };

      template<typename PRIMITIVE>
      struct Offset<PRIMITIVE,strategy::offset::MultiDistanceNumber>
      {
        typedef MultiPolygon output_type;
        
        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          PRIMITIVE _simplified;
          if (_strategy.simplify() > 0)
            gex::algorithm::simplify(_primitive,_simplified,_strategy.simplify());
          else
            _simplified = _primitive;
          
          typename OUTPUT::value_type _perimeters, _out, _offsetPolygons;

          if (!_strategy.number())
          {
            double _offset = _strategy.offset();
            while (1)
            {
              std::cout << _offset << std::endl;
              _offsetPolygons.clear();
              offset(_simplified,strategy::offset::Default(_offset),_offsetPolygons);
              if (_offsetPolygons.empty()) break;

              _offsetPolygons.update();
              _output.push_back(_offsetPolygons);
              _offset += _strategy.distance();
            }
          } else
          {
            strategy::offset::MultiDistances::set_type _set;
            for (int i = 0; i < _strategy.number(); i++)
              _set.insert(i * _strategy.distance() + _strategy.offset());
            Offset<PRIMITIVE,strategy::offset::MultiDistances>()
              (_primitive,strategy::offset::MultiDistances(_set,_strategy.simplify()),_output);
          }
        }
      };

      template<typename PRIMITIVE, typename POINT>
      struct Offset<PRIMITIVE,strategy::offset::MultiWithDetails<POINT>>
      {
        typedef MultiPolygon output_type;
        
        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          typedef typename STRATEGY::multilinestring_type multilinestring_type;
          typedef typename STRATEGY::linestring_type linestring_type;
          typedef typename STRATEGY::indices_type indices_type;
          output_type _offsetPolygons, _lastPolygons;
          auto& _width = _strategy.distance();

          if (_primitive.bounds().radius() < _width*1.05)
          {
            auto&& _c = gex::centroid(_primitive);
            linestring_type _ls;
            _ls.push_back(_c);
            _ls.push_back(_c);
            _strategy.details().push_back(_ls);
            return;
          }

          for (int i = 0; i < _strategy.number(); i++)
          {
            _offsetPolygons.clear();
            float _offset = -_width*(i+0.5);
            offset(_primitive,strategy::offset::Default(_offset),_offsetPolygons);

            auto&& _polygons = offset(_primitive,strategy::offset::Default(-_width*i));
            for (auto& _p : _offsetPolygons)
              _p.update();
            for (auto& _p : _polygons)
              _p.update();

            multilinestring_type _remaining;
            auto&& _medial_axis = convert<multilinestring_type>(medial_axis(_polygons,gex::strategy::medial_axis::Pruning(_width*0.5)));
            boost::geometry::correct(_medial_axis);
            boost::geometry::intersection(_medial_axis,_polygons,_remaining);
            _medial_axis.clear();
            join(_remaining,_medial_axis,strategy::join::ThresholdWithReverse(0.001));
            boost::geometry::correct(_medial_axis);
            boost::geometry::correct(_offsetPolygons);
            _remaining.clear();
            boost::geometry::difference(_medial_axis,_offsetPolygons,_remaining);
            _medial_axis.clear();
            boost::geometry::correct(_remaining);

            for (auto& _s : _remaining)
            {
              if (gex::sqrDistance(_s.front(),_s.back()) < 0.001 && _s.size() <= 2) continue;
              _medial_axis.push_back(_s);
            }

            for (auto& _lineString : _medial_axis)
              _strategy.details().push_back(_lineString);

            if (_offsetPolygons.empty()) break;
            
            _strategy.indices().emplace_back(_output.size(),_output.size()+_offsetPolygons.size());
            _output.insert(_output.end(),_offsetPolygons.begin(),_offsetPolygons.end());

          }
        }
      };

    }
  }
}

