#pragma once
#include <gex/prim.hpp>
#include <gex/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include "for_each.hpp"
#include "envelope.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINTSET, typename CELL>
        gex::polygon::Point const& retrieve_point(const POINTSET& _points, const CELL& _cell)
        {
          return _points[_cell.source_index()];
        }

        template<typename POINTSET, typename EDGE, typename BOUNDS, typename POINT>
        void clip_infinite_edge(
          const POINTSET& _points,
          const EDGE& edge,
          const BOUNDS& _bounds,
          std::vector<POINT>& clipped_edge)
        {
          typedef POINT point_type;
          typedef int coordinate_type; // @todo generic type here...

          auto& cell1 = *edge.cell();
          auto& cell2 = *edge.twin()->cell();
          point_type origin, direction;
          // Infinite edges could not be created by two segment sites.
          if (cell1.contains_point() && cell2.contains_point())
          {
            auto& p1 = retrieve_point(_points,cell1);
            auto& p2 = retrieve_point(_points,cell2);
            origin.x((p1.x() + p2.x()) * 0.5);
            origin.y((p1.y() + p2.y()) * 0.5);
            direction.x(p1.y() - p2.y());
            direction.y(p2.x() - p1.x());
          }
          else
          {
            return;
          }
          coordinate_type side = _bounds.size().y();
          coordinate_type koef =
            side / (std::max)(std::abs(direction.x()), std::abs(direction.y()));
          if (edge.vertex0() == NULL)
          {
            clipped_edge.emplace_back(origin.x() - direction.x() * koef,
                                      origin.y() - direction.y() * koef);
          }
          else
          {
            clipped_edge.emplace_back(edge.vertex0()->x(), edge.vertex0()->y());
          }
          if (edge.vertex1() == NULL)
          {
            clipped_edge.emplace_back(origin.x() + direction.x() * koef,
                                      origin.y() + direction.y() * koef);
          }
          else
          {
            clipped_edge.emplace_back(edge.vertex1()->x(), edge.vertex1()->y());
          }
        }

        template<typename PRIMITIVE, typename BOUNDS>
        std::vector<polygon::Segment> primitive_segments(const PRIMITIVE& _prim, const BOUNDS& _bounds)
        {
          typedef gex::prim::Segment<typename PRIMITIVE::point_type> segment_type;
          std::vector<polygon::Segment> _segments;
          gex::algorithm::for_each<segment_type>(_prim,[&](const segment_type& _segment)
          {
            _segments.emplace_back(
              gex::polygon::adapt(_segment.p0(),_bounds),
              gex::polygon::adapt(_segment.p1(),_bounds)
            );
          });

          return _segments;
        }

        template<typename SEGMENTS, typename BOUNDS, typename VORONOI>
        void voronoi_from_segments(const SEGMENTS& _segments, const BOUNDS& _bounds, VORONOI& _voronoi)
        {
          boost::polygon::voronoi_diagram<double> vd;
          boost::polygon::construct_voronoi(_segments.begin(), _segments.end(),&vd);
          bool _skip = true;
          for (auto& _edge : vd.edges())
          {
            _skip = !_skip;
            if (!_edge.is_primary()) continue; 
            if (_skip) continue;
            auto _v0 = _edge.vertex0();
            auto _v1 = _edge.vertex1();
            if (!_v0 || !_v1)
            {
              std::cout << "Infinite edge" << std::endl;
              continue;
            }
            using namespace gex::polygon;
            gex::Point2 _p0 = adapt(Point(_v0->x(),_v0->y()),_bounds);
            gex::Point2 _p1 = adapt(Point(_v1->x(),_v1->y()),_bounds);
            _voronoi.emplace_back(_p0,_p1);
          }
        }
      }

      template<typename PRIMITIVE>
      struct Voronoi {};

      template<typename POINT>
      struct Voronoi<gex::prim::Ring<POINT>> 
      {
        typedef gex::prim::Segment<POINT> segment_type;
        typedef gex::prim::MultiSegment<POINT> voronoi_type;
        typedef gex::prim::Ring<POINT> primitive_type;

        template<typename PRIMITIVE>
        void operator()(const PRIMITIVE& _prim, voronoi_type& _voronoi)
        {
          auto&& _bounds = _prim.bounds();
          detail::voronoi_from_segments(detail::primitive_segments(_prim,_bounds),_bounds,_voronoi);
        }
      };

      template<typename RING>
      struct Voronoi<gex::prim::Polygon<RING>> : 
        Voronoi<RING>
      {
        typedef gex::prim::MultiSegment<typename RING::point_type> voronoi_type;
      };

      template<typename POINT>
      struct Voronoi<gex::prim::MultiPoint<POINT>>
      {
        typedef gex::prim::Segment<POINT> segment_type;
        typedef prim::MultiSegment<POINT> voronoi_type;
        typedef gex::prim::MultiPoint<POINT> primitive_type;

        void operator()(const primitive_type& _prim, voronoi_type& _voronoi)
        {
          auto&& _bounds = envelope(_prim);

          std::vector<gex::polygon::Point> _points;
          for (auto& _p : _prim)
          {
            _points.push_back(gex::polygon::adapt(_p,_bounds));
          }

          boost::polygon::voronoi_diagram<double> vd;
          boost::polygon::construct_voronoi(_points.begin(), _points.end(),&vd);

          for (auto& _edge : vd.edges())
          {
            auto _v0 = _edge.vertex0();
            auto _v1 = _edge.vertex1();
            if (!_v0 || !_v1)
            {
              std::vector<gex::polygon::Point> _clippedEdge;
              detail::clip_infinite_edge(_points,_edge,_bounds,_clippedEdge);
              if (_clippedEdge.size() == 2)
              {
                _voronoi.emplace_back(
                  gex::polygon::adapt(_clippedEdge[0],_bounds),
                  gex::polygon::adapt(_clippedEdge[1],_bounds));
              }
              continue;
            }
            using namespace gex::polygon;
            gex::Point2 _p0 = adapt(Point(_v0->x(),_v0->y()),_bounds);
            gex::Point2 _p1 = adapt(Point(_v1->x(),_v1->y()),_bounds);
            _voronoi.emplace_back(_p0,_p1);
          }
        }
      };
    }

    using functor::Voronoi;

    template<typename PRIMITIVE, typename VORONOI>
    void voronoi(const PRIMITIVE& _prim, VORONOI& _voronoi)
    {
      Voronoi<PRIMITIVE>()(_prim,_voronoi);
    }

    template<typename PRIMITIVE>
    typename Voronoi<PRIMITIVE>::voronoi_type voronoi(const PRIMITIVE& _prim)
    {
      typename Voronoi<PRIMITIVE>::voronoi_type _voronoi;
      voronoi(_prim,_voronoi);
      return _voronoi;
    }
  }

  using algorithm::voronoi;
}





