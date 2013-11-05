#pragma once

#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/unify.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/extensions/algorithms/buffer/buffer_inserter.hpp>

#include <boost/geometry/extensions/strategies/buffer.hpp>
#include <boost/geometry/extensions/strategies/buffer_end_round.hpp>
#include "functor/Offset.hpp"
#include <boost/geometry/extensions/algorithms/remove_spikes.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT>
      struct Offset<prim::Segment<POINT>>
      {
        typedef prim::Segment<POINT> segment_type;
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;

        template<typename OFFSET>
        void operator()(const segment_type& _segment, const OFFSET& _offset, result_type& _out)
        {
          typedef prim::LineString<POINT> linestring_type;
          auto&& _lineString = convert<linestring_type>(_segment);
          Offset<linestring_type>()(_lineString,_offset,_out);
        }
      };
      
      template<typename POINT>
      struct Offset<prim::LineString<POINT>>
      {
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;
        typedef prim::LineString<POINT> linestring_type;

        template<typename OFFSET>
        void operator()(const linestring_type& _lineString, const OFFSET& _offset, result_type& _out)
        {
          if (_lineString.empty()) return;
          if ((sqrDistance(_lineString.back(),_lineString.front()) == 0.0) && 
              _lineString.size() <= 2) return;  

          if (_offset < 0.0) return;
          namespace bg = boost::geometry;
          namespace buf = bg::strategy::buffer;
          buf::join_round<POINT,POINT> _joinStrategy(16 + _offset * 10);
          buf::end_round<POINT,POINT> _endStrategy(16 + _offset * 10);
          buf::distance_symmetric<OFFSET> _distanceStrategy(_offset);
          bg::buffer_inserter<polygon_type>(_lineString, std::back_inserter(_out),
                        _distanceStrategy, 
                        _joinStrategy,
                        _endStrategy);
        }
      };
      
      template<typename POINT>
      struct Offset<prim::MultiLineString<POINT>>
      {
        typedef prim::Polygon<prim::Ring<POINT>> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;
        typedef prim::LineString<POINT> linestring_type;
        typedef prim::MultiLineString<POINT> multilinestring_type;

        template<typename OFFSET>
        void operator()(const multilinestring_type& _multiLineString, const OFFSET& _offset, result_type& _out)
        {
          result_type _lineStringPolygons;
          for (auto& _lineString : _multiLineString)
          {
            Offset<linestring_type>()(_lineString,_offset,_out);
          }
        }
      };

      template<typename POINT>
      struct Offset<prim::Ring<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef POINT point_type;
        typedef prim::Polygon<ring_type> polygon_type;
        typedef prim::MultiPolygon<polygon_type> multipolygon_type;
        typedef multipolygon_type result_type;
        typedef typename POINT::Scalar scalar_type;

        /// Returns true if offset variant was successful, false if boost polygon was used
        template<typename BOUNDS, typename OFFSET, typename SCALAR>
        bool withoutFallback(const ring_type& _in, const BOUNDS& _bounds, const OFFSET& _offset, result_type& _output, 
                        SCALAR _limit)
        {
          typedef typename ring_type::scalar_type scalar_type;

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

        template<typename OFFSET>
        void operator()(const ring_type& _in, const OFFSET& _offset, result_type& _output, 
                        Scalar _eps = 0.001)
        {
          typedef typename ring_type::bounds_type bounds_type;
          
          ring_type _simplified;
          auto& _bounds = _in.bounds();
          auto _limit = _bounds.size().norm() * _eps;
          boost::geometry::simplify(_in,_simplified,_limit);
          _simplified.update();

          if (!withoutFallback(_simplified,_in.bounds(),_offset,_output,_limit))
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
      struct Offset<prim::Polygon<RING>>
      {
        typedef prim::MultiPolygon<prim::Polygon<RING>> result_type;

        template<typename OFFSET>
        void operator()(
          const prim::Polygon<RING>& _in, const OFFSET& _offset, 
          result_type& _out, 
          Scalar _eps = 0.001)
        {
          typedef Offset<RING> offset_type;
          if (_in.holes().empty())
          {
            offset_type()(_in.boundary(),_offset,_out,_eps);
            return;
          }
          
          auto& _bounds = _in.bounds();
          Scalar _limit = _bounds.size().norm() * _eps;
          prim::Polygon<RING> _simplified;
          boost::geometry::simplify(_in,_simplified,_limit);
          _simplified.update();

          result_type _boundary, _holes;
          if (!offset_type().withoutFallback(_simplified.boundary(),_bounds,_offset,_boundary,_limit))
          {
            boostPolygonFallback(_simplified,_bounds,_offset,_out,_limit);
            return;
          }
 
          for (auto& _r : _simplified.holes())
          {
            result_type _holesBuffer;
            if (!offset_type().withoutFallback(_r,_bounds,-_offset,_holesBuffer,_limit))
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
          result_type& _out, 
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
      struct Offset<prim::MultiPolygon<POLYGON>>
      {
        typedef prim::MultiPolygon<POLYGON> result_type;

        template<typename OFFSET>
        void operator()(const result_type& _in, const OFFSET& _offset, result_type& _out)
        {
          result_type _offsetPolygons;
          for (auto& _p : _in)
          {
            result_type _tmp;
            Offset<POLYGON>()(_p,_offset,_tmp);
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
    
    }

    using functor::Offset;

    template<typename IN, typename OFFSET, typename OUT>
    void offset(const IN& _in, const OFFSET& _offset, OUT& _out)
    {
      Offset<IN>()(_in,_offset,_out);
    }

    template<typename IN, typename OFFSET>
    typename Offset<IN>::result_type offset(const IN& _in, const OFFSET& _offset)
    {
      typename Offset<IN>::result_type _out;
      offset(_in,_offset,_out);
      return _out;
    }
  }

  using algorithm::offset; 
}

