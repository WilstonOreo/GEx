#pragma once

#include "gex/prim.hpp"
#include "voronoi.hpp"
#include <gex/index.hpp>
#include "convert.hpp"
#include "distance.hpp"
#include "within.hpp"
#include "join.hpp"

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
          typedef typename PRIMITIVE::point_type point_type;
          SEGMENTS _segments;
          typedef index::RTreeNode<typename PRIMITIVE::bounds_type,point_type const*> node_type;
          typedef index::RTree<node_type> rtree_type;
          rtree_type _rtree;
          std::vector<node_type> _nodes;

          const float _epsilon = 0.001;
          auto _size = _prim.bounds().size() * _epsilon;

          for_each<point_type>(_prim,[&](const point_type& _p)
          {
            typename PRIMITIVE::bounds_type _bounds(_p +(-_size),_p + _size); 
            _nodes.emplace_back(_bounds,&_p);
          });
          _rtree.insert(_nodes.begin(),_nodes.end());
          
          for (auto& _segment : _voronoi)
          {
            _nodes.clear();
            if (!within(_segment,_prim)) continue;
            _rtree.query(index::intersects(_segment.bounds()),std::back_inserter(_nodes));
            bool _insert = true;
            for (auto& _node : _nodes)
            {
              auto _p = _node.second;
              if ((sqrDistance(*_p,_segment.p0()) <= _epsilon) ||
                  (sqrDistance(*_p,_segment.p1()) <= _epsilon))
              {
                _insert = false;
                break;
              }
            }
            if (_insert)
              _segments.push_back(_segment);
          }
          return _segments;
        }
      }

      
      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::SegmentsOnly>
      {
        typedef prim::MultiSegment<POINT> output_type;
        typedef prim::Ring<POINT> ring_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY, output_type& _medial_axis)
        {
          auto&& _voronoi = voronoi(_ring);
          _medial_axis = detail::medial_axis_remove_segments_from_voronoi(_ring,_voronoi);
        }
      };
      
      template<typename RING>
      struct MedialAxis<prim::Polygon<RING>,medial_axis::SegmentsOnly> : 
        MedialAxis<RING,medial_axis::SegmentsOnly>
      {
        typedef prim::MultiSegment<typename RING::point_type> output_type;
      };

      template<typename POINT>
      struct MedialAxis<prim::Ring<POINT>,medial_axis::Joined>
      {
        typedef prim::MultiLineString<POINT> output_type;
        typedef prim::Ring<POINT> ring_type;

        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _ring, STRATEGY, output_type& _medial_axis)
        {
          typedef medial_axis::SegmentsOnly segment_strategy;
          typedef MedialAxis<ring_type,segment_strategy> segment_axis_type;
          typedef typename segment_axis_type::output_type segments_type;
          segments_type _segments;
          segment_axis_type()(_ring,segment_strategy(),_segments);
          join(_segments,_medial_axis,strategy::ThresholdWithReverse(0.001));          
        }
      };

      template<typename RING>
      struct MedialAxis<prim::Polygon<RING>,medial_axis::Joined> : 
        MedialAxis<RING,medial_axis::Joined>
      {
        typedef RING ring_type;
        typedef typename ring_type::point_type point_type;
        typedef prim::Polygon<RING> polygon_type;
        typedef prim::MultiLineString<point_type> output_type;
/*
        template<typename PRIMITIVE, typename STRATEGY>
        void operator()(const PRIMITIVE& _polygon, STRATEGY, output_type& _medial_axis)
        {
          auto&& _voronoi = voronoi(_polygon);
          auto&& _segments = detail::medial_axis_remove_segments_from_voronoi(_polygon,_voronoi);
          _medial_axis = convert<output_type>(_segments);
          join(_segments,_medial_axis,strategy::ThresholdWithReverse(0.001));
        }*/
      };
    }

    using functor::MedialAxis;

    template<typename PRIMITIVE, typename STRATEGY, typename SKELETON>
    void medial_axis(const PRIMITIVE& _primitive, STRATEGY _strategy, SKELETON& _medial_axis)
    {
      MedialAxis<PRIMITIVE,STRATEGY>()(_primitive,_medial_axis);
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
