#pragma once

#include "gex/prim.hpp"
#include "voronoi.hpp"
#include <gex/index.hpp>
#include "convert.hpp"
#include "distance.hpp"
#include "within.hpp"
#include "join.hpp"
#include "closestPoint.hpp"
#include "boost.hpp"

namespace gex
{
  namespace strategy
  {
    namespace medial_axis
    {
      struct Joined {};
      struct SegmentsOnly {};
    
      struct ScaleAxis
      {
        ScaleAxis(float _s = 1.0) : 
          s_(_s) {}
        TBD_PROPERTY_RO(float,s)
      };
    }
  }

  namespace algorithm
  {
    namespace functor
    {
      using namespace gex::strategy;

      template<typename PRIMITIVE, typename STRATEGY>
      struct MedialAxis {};

      namespace detail
      {
        template<typename PRIMITIVE, typename SEGMENTS>
        SEGMENTS medial_axis_remove_segments_from_voronoi(const PRIMITIVE& _prim, const SEGMENTS& _voronoi)
        {
          auto&& _lineStrings = gex::convert<gex::MultiLineString>(_voronoi);
          boost::geometry::correct(_lineStrings);
          gex::MultiLineString _newLineStrings;
          boost::geometry::intersection(_lineStrings,_prim,_newLineStrings);
          SEGMENTS _output;
          for_each<gex::Segment>(_newLineStrings,[&](const gex::Segment& _segment)
          {
            auto&& _p0 = gex::closestPoint(_prim,_segment.front()); 
            auto&& _p1 = gex::closestPoint(_prim,_segment.back()); 
            auto&& _dist0 = gex::distance(_p0,_segment.front());
            auto&& _dist1 = gex::distance(_p1,_segment.back());
            auto _dist = std::min(_dist0,_dist1);
            if (_dist < 0.001) return;

            _output.push_back(_segment);
          });
          return _output;
        }

        template<typename PRIMITIVE, typename OFFSET, typename SEGMENTS, typename OUTPUT>
        void medial_axis_scale_offset(const PRIMITIVE& _primitive, const OFFSET& _offset, const SEGMENTS& _segments, OUTPUT& _output)
        {
          polygon::MultiPolygon _multiPolygon;

          auto&& _bounds = _primitive.bounds();

          using namespace boost::polygon::operators;
          _multiPolygon += polygon::adapt(_primitive,_bounds);
          for (auto& _segment : _segments)
          {
            auto&& _p0 = gex::closestPoint(_primitive,_segment.front()); 
            auto&& _p1 = gex::closestPoint(_primitive,_segment.back()); 
            auto&& _dist0 = gex::distance(_p0,_segment.front());
            auto&& _dist1 = gex::distance(_p1,_segment.back());
            auto _dist = std::min(_dist0,_dist1);
            _multiPolygon += polygon::adapt(gex::offset(_segment,_dist + _offset),_bounds);
          }

          _output = polygon::adapt(_multiPolygon,_bounds);
        }
      }

      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::SegmentsOnly>
      {
        typedef prim::MultiSegment<POINT> output_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY, output_type& _medial_axis)
        {
          auto&& _voronoi = voronoi(_ring);
          _medial_axis = detail::medial_axis_remove_segments_from_voronoi(_ring,_voronoi);
        }
      };
      
      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::Joined>
      {
        typedef prim::MultiLineString<POINT> output_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY, output_type& _medial_axis)
        {
          typedef medial_axis::SegmentsOnly segment_strategy;
          typedef MedialAxis<PRIMITIVE,segment_strategy> segment_axis_type;
          typedef typename segment_axis_type::output_type segments_type;
          segments_type _segments;
          segment_axis_type()(_ring,segment_strategy(),_segments);
          join(_segments,_medial_axis,strategy::ThresholdWithReverse(0.001));          
        }
      };
      
      template<typename RING, typename STRATEGY>
      struct MedialAxis<prim::Polygon<RING>,STRATEGY> : 
        MedialAxis<RING,STRATEGY>
      {
        typedef typename MedialAxis<RING,STRATEGY>::output_type output_type;
      };

      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::ScaleAxis>
      {
        typedef prim::MultiLineString<POINT> output_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY _s, output_type& _medial_axis)
        {
          typedef medial_axis::SegmentsOnly segment_strategy;
          PRIMITIVE _simplifiedRing;
          gex::algorithm::simplify(_ring,_simplifiedRing,_s.s()*0.25);
          _simplifiedRing.update();

          typedef MedialAxis<PRIMITIVE,segment_strategy> segment_axis_type;
          typedef typename segment_axis_type::output_type segments_type; 
          segments_type _segments;
          segment_axis_type()(_simplifiedRing,segment_strategy(),_segments);
          
          gex::MultiPolygon _multiPolygon;
          detail::medial_axis_scale_offset(_simplifiedRing,_s.s(),_segments,_multiPolygon);
          if (_multiPolygon.size() != 1) return;

          _multiPolygon[0].update();
          gex::Polygon _polygon;
          gex::algorithm::simplify(_multiPolygon[0],_polygon,_s.s()*0.5);
          _polygon.update();
          segments_type _scaleAxis;
          MedialAxis<gex::Polygon,segment_strategy>()(_polygon,segment_strategy(),_scaleAxis);
          _segments = detail::medial_axis_remove_segments_from_voronoi(_ring,_scaleAxis);
          join(_segments,_medial_axis,strategy::ThresholdWithReverse(0.001));          
        }
      };
    }

    using functor::MedialAxis;

    template<typename PRIMITIVE, typename STRATEGY, typename SKELETON>
    void medial_axis(const PRIMITIVE& _primitive, STRATEGY _strategy, SKELETON& _medial_axis)
    {
      MedialAxis<PRIMITIVE,STRATEGY>()(_primitive,_strategy,_medial_axis);
    }

    template<typename PRIMITIVE, typename STRATEGY>
    typename MedialAxis<PRIMITIVE,STRATEGY>::output_type medial_axis(const PRIMITIVE& _primitive, STRATEGY _strategy)
    {
      typename MedialAxis<PRIMITIVE,STRATEGY>::output_type  _medial_axis;
      medial_axis(_primitive,_strategy,_medial_axis);
      return _medial_axis;
    }
  }

  using algorithm::medial_axis;
}
