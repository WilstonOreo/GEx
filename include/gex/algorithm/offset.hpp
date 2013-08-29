#pragma once

#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/unify.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/extensions/algorithms/buffer/buffer_inserter.hpp>

#include <boost/geometry/extensions/strategies/buffer.hpp>
#include <boost/geometry/extensions/strategies/buffer_end_round.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Offset {};

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
          buf::join_round<POINT,POINT> _joinStrategy;
          buf::end_round<POINT,POINT> _endStrategy;
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
        typedef prim::Polygon<ring_type> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;
        typedef prim::LineString<POINT> linestring_type;

        template<typename OFFSET>
        void operator()(const ring_type& _ring, const OFFSET& _offset, result_type& _out)
        {
          if (_offset == 0.0) 
          {
            _out.emplace_back(_ring);
            return;
          }

          auto&& _boundary = convert<linestring_type>(_ring);
          result_type _offsetBoundary;
          Offset<linestring_type>()(_boundary,std::abs(_offset),_offsetBoundary);
          if (_offset > 0.0)
          {
            union_(_offsetBoundary,_ring,_out);
          } 
          else // _offset < 0.0
          {
            difference(_ring,_offsetBoundary,_out);
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
          result_type& _out)
        {
          result_type _boundary, _holes;
          Offset<RING>()(_in.boundary(),_offset,_boundary);
          for (auto& _r : _in.holes())
          {
            Offset<RING>()(_r,-_offset,_holes);
          }
          for(auto& _hole : _holes) 
            _hole.update();

          if (_offset < 0.0)
            _holes = unify(_holes);
          difference(_boundary,_holes,_out);
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
            Offset<POLYGON>()(_p,_offset,_offsetPolygons);
          }
          if (_offset > 0.0)
          {
            _offsetPolygons = unify(_offsetPolygons);
          }
          _out.insert(_out.end(),_offsetPolygons.begin(),_offsetPolygons.end());
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

