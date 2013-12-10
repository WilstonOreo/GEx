#pragma once

#include <gex/prim.hpp>
#include <gex/strategy/medial_axis.hpp>
#include <gex/index.hpp>

#include "voronoi.hpp"
#include "convert.hpp"
#include "distance.hpp"
#include "within.hpp"
#include "join.hpp"
#include "closestPoint.hpp"
#include "boost.hpp"


namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      using namespace gex::strategy;

      template<typename PRIMITIVE, typename STRATEGY>
      struct MedialAxis {};
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
}

#include "functor/Pruning.hpp"


namespace gex
{
  namespace algorithm 
  {
    namespace functor
    {
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
            _output.push_back(_segment);
          });
          return _output;
        }

        template<typename PRIMITIVE, typename SCALE, typename SEGMENTS, typename OUTPUT>
        void medial_axis_scale_offset(const PRIMITIVE& _primitive, const SCALE& _s, const SEGMENTS& _segments, OUTPUT& _output)
        {
          polygon::MultiPolygon _multiPolygon;

          auto&& _bounds = _primitive.bounds();

          using namespace boost::polygon::operators;
          _multiPolygon += polygon::adapt(_primitive,_bounds);
for (auto& _segment : _segments)
          {
            auto&& _p0 = closestPoint(_primitive,_segment.p0());
            auto&& _p1 = closestPoint(_primitive,_segment.p1());
            auto&& _dist0 = distance(_p0,_segment.p0());
            auto&& _dist1 = distance(_p1,_segment.p1());
            _multiPolygon += polygon::adapt(
                               offset(_segment,gex::strategy::offset::NonUniform<gex::Segment>(std::make_pair(_dist0+_s,_dist1+_s))),_bounds);
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
//          _medial_axis = voronoi(_ring);
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
          join(_segments,_medial_axis,strategy::join::ThresholdWithReverse(0.001));
        }
      };

      template<typename RING, typename STRATEGY>
      struct MedialAxis<prim::Polygon<RING>,STRATEGY> :
          MedialAxis<RING,STRATEGY>
      {
        typedef typename MedialAxis<RING,STRATEGY>::output_type output_type;
      };
      
      template<typename POLYGON, typename STRATEGY>
      struct MedialAxis<prim::MultiPolygon<POLYGON>,STRATEGY> :
          MedialAxis<POLYGON,STRATEGY>
      {
        typedef typename MedialAxis<POLYGON,STRATEGY>::output_type output_type;
        
        template<typename P, typename S>
        void operator()(const P& _prim, S _s, output_type& _medial_axis)
        {
          for (auto& _p : _prim)
          {
            auto&& _m = medial_axis(_p,_s);
            for (auto& _element : _m)
            {
              _medial_axis.push_back(_element);
            }
          }
        }
      };

      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::ScaleAxis>
      {
        typedef prim::MultiLineString<POINT> output_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY _s, output_type& _medial_axis)
        {
          typedef medial_axis::SegmentsOnly segment_strategy;
          typedef MedialAxis<PRIMITIVE,segment_strategy> segment_axis_type;
          typedef typename segment_axis_type::output_type segments_type;
          segments_type _segments;
          gex::MultiPolygon _multiPolygon;

          segment_axis_type()(_ring,segment_strategy(),_segments);
          detail::medial_axis_scale_offset(_ring,_s.s(),_segments,_multiPolygon);
          if (_multiPolygon.size() != 1) return;

          _multiPolygon[0].update();
          gex::Polygon _polygon;
          gex::algorithm::simplify(_multiPolygon[0],_polygon,_s.s());
          _polygon.update();
          segments_type _scaleAxis;
          MedialAxis<gex::Polygon,segment_strategy>()(_polygon,segment_strategy(),_scaleAxis);

          _segments = detail::medial_axis_remove_segments_from_voronoi(_ring,_scaleAxis);
          join(_segments,_medial_axis,strategy::join::ThresholdWithReverse(0.001));
        }
      };

      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::Pruning>
      {
        typedef prim::MultiSegment<POINT> output_type;

        template<typename P, typename STRATEGY, typename MEDIAL_AXIS>
        void operator()(const P& _p, STRATEGY _strategy, MEDIAL_AXIS& _output)
        {
          auto&& _medial_axis = medial_axis(_p,gex::strategy::medial_axis::SegmentsOnly());
          auto _factor = _strategy.factor();
          auto&& _c = centroid(_p);
          P _simplified;
          gex::algorithm::simplify(_p,_simplified,_factor*0.5);
          _simplified.update();
          if (_simplified.boundary().size() <= 2)
          {
            _output.clear();
            _output.emplace_back(_c,_c);
            return;
          }

          auto&& _secPruned = prune_secondary(_simplified,_medial_axis);
          _output = prune_primary(_p,_factor,_secPruned);
          if (_output.empty())
          {
            _output.emplace_back(_c,_c);
          }
        }
      };
    }
  }

  using algorithm::medial_axis;
}
