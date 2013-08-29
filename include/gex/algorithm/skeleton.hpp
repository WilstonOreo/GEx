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
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Skeleton
      {
      };

      namespace detail
      {
        template<typename PRIMITIVE, typename SEGMENTS>
        SEGMENTS skeleton_remove_segments_from_voronoi(const PRIMITIVE& _prim, const SEGMENTS& _voronoi)
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
      struct Skeleton<prim::Ring<POINT>>
      {
        typedef prim::MultiLineString<POINT> output_type;
        typedef prim::Ring<POINT> ring_type;

        void operator()(const ring_type& _ring, output_type& _skeleton)
        {
          auto&& _voronoi = voronoi(_ring);
          auto&& _segments = detail::skeleton_remove_segments_from_voronoi(_ring,_voronoi);
          
          join(_segments,_skeleton,0.0001);
          
          //_skeleton = convert<output_type>(_segments);
        }
      };

      template<typename RING>
      struct Skeleton<prim::Polygon<RING>> 
      {
        typedef RING ring_type;
        typedef typename ring_type::point_type point_type;
        typedef prim::Polygon<RING> polygon_type;
        typedef prim::MultiLineString<point_type> output_type;

        void operator()(const polygon_type& _polygon, output_type& _skeleton)
        {
          auto&& _voronoi = voronoi(_polygon);
          auto&& _segments = detail::skeleton_remove_segments_from_voronoi(_polygon,_voronoi);
          //_skeleton = convert<output_type>(_segments);
          join(_segments,_skeleton,0.0001);
        }
      };
    }

    using functor::Skeleton;

    template<typename PRIMITIVE, typename SKELETON>
    void skeleton(const PRIMITIVE& _primitive, SKELETON& _skeleton)
    {
      Skeleton<PRIMITIVE>()(_primitive,_skeleton);
    }

    template<typename PRIMITIVE>
    typename Skeleton<PRIMITIVE>::output_type skeleton(const PRIMITIVE& _primitive)
    {
      typename Skeleton<PRIMITIVE>::output_type  _skeleton;
      Skeleton<PRIMITIVE>()(_primitive,_skeleton);
      return _skeleton;
    }
  }

  using algorithm::skeleton;
}
