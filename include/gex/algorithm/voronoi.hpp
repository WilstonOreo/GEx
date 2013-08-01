#include <gex/prim.hpp>
#include <gex/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include "for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      struct Voronoi
      {
        template<typename POINTS, typename BOUNDS, typename SEGMENTS>
        void operator()(const POINTS& _pointSet, const BOUNDS& _bounds, SEGMENTS& _segments)
        {
          typedef typename PRIMITIVE::point_type point_type;
          using polygon::adapt;
          std::vector<polygon::Point2> _points;
          for (auto& _p : _points)
            _points.push_back(polygon::adapt(_p,_bounds));
          generate(_points,_bounds,_segments);
        }

        template<typename PRIMITIVE, typename SEGMENTS>
        void operator()(const PRIMITIVE& _prim, SEGMENTS& _segments)
        {
          typedef typename PRIMITIVE::point_type point_type;
          std::vector<polygon::Point2> _points;
          _points.reserve(num<point_type>(_prim));
          auto _bounds = _prim.bounds();
          for_each<point_type>(_prim,[&](const point_type& _p)
          {
            _points.push_back(polygon::adapt(_p,_bounds));
          });
          generate(_points,_bounds,_segments);
        }

      private:
        template<typename POINTSET, typename CELL>
        polygon::Point2 const& retrieve_point(const POINTSET& _points, const CELL& _cell)
        {
          return _points[_cell.source_index()];
        }

        template<typename POINTSET, typename EDGE, typename BOUNDS, typename POINT>
        void clip_infinite_edge(
          const POINTSET& _points,
          const EDGE& _edge,
          const BOUNDS& _bounds,
          std::vector<POINT>& _clippedEdge)
        {
          typedef POINT point_type;
          typedef int coordinate_type; // @todo generic type here...

          auto& _cell1 = *_edge.cell();
          auto& _cell2 = *_edge.twin()->cell();
          point_type _origin, _direction;
          // Infinite edges could not be created by two segment sites.
          if (_cell1.contains_point() && _cell2.contains_point())
          {
            auto& _p1 = retrieve_point(_points,_cell1);
            auto& _p2 = retrieve_point(_points,_cell2);
            _origin.x((_p1.x() + _p2.x()) * 0.5);
            _origin.y((_p1.y() + _p2.y()) * 0.5);
            direction.x(_p1.y() - _p2.y());
            direction.y(_p2.x() - _p1.x());
          }
          else
          {
            return;
          }
          coordinate_type _side = _bounds.size().y();
          coordinate_type _koef =
            side / (std::max)(std::abs(_direction.x()), std::abs(_direction.y()));
          
          point_type _point(_origin.x() - _direction.x() * _koef,
                            _origin.y() - _direction.y() * _koef);
          if (!_edge.vertex0())
          {
            _clippedEdge.emplace_back(_point);
          }
          else
          {
            _clippedEdge.emplace_back(_edge.vertex0()->x(), _edge.vertex0()->y());
          }
          if (!_edge.vertex1())
          {
            _clippedEdge.emplace_back(_point);
          }
          else
          {
            _clippedEdge.emplace_back(_edge.vertex1()->x(), _edge.vertex1()->y());
          }
        }

        template<typename BOUNDS, typename SEGMENTS>
        void generate(const std::vector<polygon::Point2>& _points, const BOUNDS&, SEGMENTS& _segments)
        {
          typedef typename PRIMITIVE::point_type point_type;
          using polygon::adapt;
          
          boost::polygon::voronoi_diagram<double> vd;
          boost::polygon::construct_voronoi(_points.begin(), _points.end(),&vd);

          /// Retrieve edges from voronoi
          for (auto& _edge : vd.edges())
          {
            auto _v0 = _edge.vertex0(), _v1 = _edge.vertex1();
            if (!_v0 || !_v1)
            {
              std::vector<polygon::Point2> _cEdge;
              clip_infinite_edge(_points,_edge,_bounds,_cEdge);
              if (_cEdge.size() == 2)
              {
                _segments.emplace_back(
                  adapt(_cEdge[0],_bounds),adapt(_cEdge[1],_bounds));
              }
              continue;
            }

            auto&& _p0 = adapt(polygon::Point2(_v0->x(),_v0->y()),_bounds);
            auto&& _p1 = adapt(polygon::Point2(_v1->x(),_v1->y()),_bounds);
            _segments.emplace_back(_p0,_p1);
          }
        }
      };
    }

    using functor::Voronoi;

    template<typename PRIMITIVE, typename SEGMENTS>
    void voronoi(const PRIMITIVE& _prim, SEGMENTS& _segments)
    {
      Voronoi()(_prim,_segments);
    }
    
    template<typename POINTS, typename BOUNDS, typename SEGMENTS>
    void voronoi(const POINTS& _points, const BOUNDS& _bounds, SEGMENTS& _segments)
    {
      Voronoi()(_points,_bounds,_segments);
    }

    template<typename PRIMITIVE>
    std::vector<prim::Segment<typename PRIMITIVE::model_type>> voronoi(const PRIMITIVE& _prim)
    {
      std::vector<prim::Segment<typename PRIMITIVE::model_type>> _segments;
      voronoi(_prim,_segments);
      return _segments;
    }

    template<typename POINTS, typename BOUNDS>
    std::vector<prim::Segment<typename PRIMITIVE::model_type>> 
      voronoi(const POINTS& _points, const BOUNDS& _bounds)
    {
      std::vector<prim::Segment<typename PRIMITIVE::model_type>> _segments;
      voronoi(_points,_bounds,_segments);
      return _segments;
    }
  }
}





