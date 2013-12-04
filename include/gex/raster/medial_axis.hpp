#pragma once

#include "Image.hpp"
#include "for_each.hpp"
#include "draw_circle.hpp"

#include <gex/misc.hpp>

namespace gex
{
  namespace raster
  {
    namespace strategy
    {
      namespace medial_axis
      {
        struct Default {};
      }
    }

    GEX_FUNCTOR(MedialAxis,medial_axis)

    namespace functor
    {
      struct MedialAxisPoint
      {
        typedef gex::Point2 point_type;
        typedef point_type::Scalar scalar_type;

        MedialAxisPoint() {};
        MedialAxisPoint(
            const scalar_type& _distance,
            const point_type& _point) :
          distance_(_distance),
          point_(_point) {}

        TBD_PROPERTY_REF(scalar_type,distance)
        TBD_PROPERTY_REF(point_type,point)
      };

      template<typename POINT, typename POINT_RTREE, typename POINT_PAIR>
      bool get_closest_points(const POINT& _point, const POINT_RTREE& _pointsRTree, POINT_PAIR& _pointPair)
      {
        typedef POINT point_type;
        typedef typename point_type::Scalar scalar_type;
        typedef std::pair<point_type,size_t> node_type;
        typedef gex::index::RTree<node_type> rtree_type;

        std::vector<node_type> _first; // Contains nearest node

        auto& _rtree = _pointsRTree.second;
        auto& _points = _pointsRTree.first;
        _rtree.query(gex::index::nearest(_point,1),std::back_inserter(_first));

        if (_first.empty()) return false;

        auto& _nearest = _points[_first.front().second];
        auto _dist = gex::distance(_nearest,_point);
        if (_dist < 2.0) return false;

        gex::Vec2 _m(_dist+2.0,_dist+2.0);
        gex::Bounds2 _bounds(_point - _m,_point + _m);

        auto _equalDistance = [&_points,&_nearest,&_point,&_dist](const node_type& _node)
        {
          if (&_points[_node.second] == &_nearest) return false;
          if (gex::distance(_points[_node.second],_nearest) < 2.0) return false;
          auto _nodeDist = gex::distance(_node.first,_point); 
          return _nodeDist - _dist < 4.0;
        };

        std::vector<node_type> _second; // Contains a list of _candidates of second nearest node
        _rtree.query(gex::index::covered_by(_bounds) && gex::index::satisfies(_equalDistance),
            std::back_inserter(_second));

        if (_second.empty()) return false;
        
        typedef base::Curve<point_type> curve_type;

        scalar_type _maxBackAngle = 45.0, _maxFrontAngle = 45.0;
        point_type const* _secondPoint = nullptr; //&_second.front();

        gex::Segment _segment(_point,_nearest);
        auto&& _normal = _segment.normal();
        bool _frontSide = false, _backSide = false;

        for (auto& _node : _second)
        {
          auto& _p = _points[_node.second];
          curve_type _curve(_p,_point,_nearest);
          auto _angle = _curve.angle();

          auto _dot = (_p - _point).dot(_normal);

          if (_dot >= 0)
          {
            _frontSide = true;
            if (_angle <= _maxFrontAngle) 
            {
              _maxFrontAngle = _angle;
              _secondPoint = &_p;
            }
          }
          if (_dot <= 0)
          {
            _backSide = true;
            if (_angle <= _maxBackAngle) 
            {
              _maxBackAngle = _angle;
              _secondPoint = &_p;
            }
          }
        }

        if (!_frontSide || !_backSide) 
          _secondPoint = nullptr;

        if (!_secondPoint) return false;

        _pointPair.first = _nearest;
        _pointPair.second = *_secondPoint;
        return true;
      }

      template<typename MEDIAL_AXIS, typename MULTI_SEGMENT>
      void connect_medial_axis(const MEDIAL_AXIS& _points, MULTI_SEGMENT& _output)
      {
        /// TODO
        auto _tmp = _points;

        typedef typename MEDIAL_AXIS::value_type point_type;
        std::sort(_tmp.begin(),_tmp.end(),[](const point_type& _lhs, const point_type& _rhs)
        {
          return _lhs.distance() < _rhs.distance();
        });
/*
        for (auto& _point : _tmp)
        {
          if (find_nearest(_point,_tmp,_rtree))
          {
            
          }
        }*/
      }

      template<typename MEDIAL_AXIS_POINTS, typename FACTOR, typename IMAGE>
      void scaled_balls(const MEDIAL_AXIS_POINTS& _points, FACTOR _s, IMAGE& _image)
      {
        for (auto& _p : _points)
        {
          draw_circle(_p.distance()*_s,_p.point(),_image);
        }

      }
      
      using namespace strategy;
      
      template<typename MEDIAL_AXIS_POINTS, typename THRESHOLD, typename IMAGE>
      void scale_below_threshold(
          const MEDIAL_AXIS_POINTS& _points, 
          const THRESHOLD& _threshold,
          IMAGE& _image)
      {
        for (auto& _p : _points)
        {
          draw_circle(_threshold,_p.point(),_image);
        }
      }

      template<typename IMAGE>
      struct MedialAxis<IMAGE,medial_axis::Default>
      {
        typedef std::vector<functor::MedialAxisPoint> output_type;

        template<typename STRATEGY>
        void operator()(const IMAGE& _in, STRATEGY, output_type& _out)
        {
          auto&& _pointsRTree = raster::edge(_in,strategy::edge::PointsRTree(0.0));
          typedef typename IMAGE::pixel_type pixel_type;
          typedef gex::Point2 point_type;
          typedef typename point_type::Scalar scalar_type;

          raster::for_each(_in,strategy::for_each::Pixel(),
              [&](const pixel_type& _pixel, size_t _x, size_t _y)
          { 
            if (_pixel == 0) return;
            
            point_type _p(_x+0.5,_y+0.5);

            std::pair<point_type,point_type> _pointPair;
            if (get_closest_points(_p,_pointsRTree,_pointPair))
            {
              scalar_type _minDist = std::min(gex::distance(_pointPair.first,_p),gex::distance(_pointPair.second,_p)); 
              _out.emplace_back(_minDist,_p);
            }
          });
          std::cout << _out.size() << std::endl;
        };
      };
    }
  }
}
