#pragma once

#include "gex/algorithm/unify.hpp"
#include "gex/algorithm/intersection.hpp"
#include "gex/algorithm/for_each.hpp"
#include <gex/algorithm/within.hpp>

#include <gex/polygon.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINT, typename ANGLE, typename RADIUS, typename LIMIT, typename OUTPUT>
        void buildArc(
          const POINT& _point,
          const ANGLE& _a0,
          const ANGLE& _a1,
          const RADIUS& _radius,
          const LIMIT& _limit,
          OUTPUT& _out)
        {
          typedef POINT point_type;
          typedef typename point_type::Scalar scalar_type;
          typedef POINT vec_type; 

          auto _arcFrac = std::abs(_a1 - _a0) * util::invPi2();
          int _steps = (int)(_arcFrac * M_PI / std::acos(1.0 - _limit / std::abs(_radius)));
          if (_steps < 2) _steps = 2;
          else if (_steps > (int)(222.0 * _arcFrac)) _steps = (int)(222.0 * _arcFrac);

          POINT _v(0.0,0.0);
          double _c, _s;
          algorithm::util::sinCos(_a0,_v(Y),_v(X));
          algorithm::util::sinCos((_a1 - _a0) / _steps, _s, _c);

          _out.reserve(_out.size() + _steps+1);
          for (int _i = 0; _i <= _steps; ++_i)
          {
            _out.push_back(POINT(_point.x() + _v.x()*_radius,_point.y() + _v.y()*_radius));
            auto _x2 = _v.x();
            _v(X) = _v.x() * _c - _s * _v.y();  // cross product
            _v(Y) = _x2 * _s + _v.y() * _c; // dot product
          }
        }

        template<typename POINT, typename RING>
        void generateJunction( const POINT& _p0,
                               const POINT& _p1,
                               const POINT& _p2,
                               typename POINT::Scalar _offset,
                               typename POINT::Scalar _limit,
                               RING& _ring
                             )
        {
          typedef POINT point_type;
          typedef typename POINT::Scalar scalar_type;
          typedef typename RING::vec_type vec_type;

          _ring.clear();
          if ((_p0 - _p1).norm() < _limit)
            return;

          using namespace algorithm::util;

          // Normals
          auto _n0 = getNormal(_p0,_p1).normalized(),
               _n1 = getNormal(_p1,_p2).normalized();

          // Scaled normals;
          vec_type _sn0(_n0);
          _sn0 *= _offset;
          vec_type _sn1(_n1);
          _sn1 *= _offset;

          auto _front = _p0;
          auto _back = (_p2 - _p1).normalized() * _offset + _p1;

          _ring.push_back(_front - _sn0);
          _ring.push_back(_front + _sn0);
          double _a0 = std::atan2(_n1.y(),_n1.x()),
                 _a1 = std::atan2(_n0.y(),_n0.x());

          if (_a1 < _a0) _a1 += M_PI * 2.0;

          if (_a1 - _a0 < M_PI)
          {
            detail::buildArc(_p1,_a1,_a0,_offset,_limit,_ring);
          }
          else
          {
            gex::prim::Segment<POINT> _seg0(_front + _sn0,
                                            _p1 + _sn0),
                                                _seg1(_p1 + _sn1,
                                                    _back + _sn1);
            POINT _iPoint;
            if (lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            } else
            {
              _ring.push_back(_p1 + _sn0);
            }
          }
      
         
          if (_a1 - _a1 > M_PI)
          _ring.push_back(_back);

          _a0 += M_PI;
          _a1 -= M_PI;

          if (_a0 - _a1 < M_PI)
          {
            detail::buildArc(_p1,_a0,_a1,_offset,_limit,_ring);
          }
          else
          {
            gex::prim::Segment<POINT>
            _seg0(_front - _sn0,_p1 - _sn0),
                  _seg1(_p1 - _sn1,_back - _sn1);
            POINT _iPoint;
            if (lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            } else
            {
              _ring.push_back(_p1 - _sn0);
            }
          }

          _ring.update();
        }


        template<typename RING, typename OFFSET, typename LIMIT, typename POLYGON>
        bool generateOffsetPolygon(
          const RING& _ring,
          const OFFSET& _offset,
          const LIMIT& _limit,
          POLYGON& _polygon)
        {
          RING _junction;
          typedef typename RING::point_type point_type;
          typedef typename RING::scalar_type scalar_type;
          typedef base::Curve<point_type> curve_type;
          typedef gex::prim::MultiPolygon<POLYGON> multipolygon_type;
          _junction.reserve(64);
          multipolygon_type _offsetPolygons;

          bool _skip = false;
          gex::for_each<curve_type>(_ring,[&](const curve_type& _curve)
          {
            if (_skip) return;
            _offsetPolygons.clear();
            gex::algorithm::functor::detail::generateJunction(
              _curve.p0(),
              _curve.p1(),
              _curve.p2(),
              _offset,_limit,_junction);
            
            boost::geometry::correct(_junction);
            gex::algorithm::union_(_polygon,_junction,_offsetPolygons);              
            
            if (_offsetPolygons.size() != 1)
            {
              _skip = true;
              return;
            } 
            _polygon = _offsetPolygons[0];
            _polygon.update();
          });

          if (_skip || _polygon.boundary().empty()) 
            return false;

          typedef gex::prim::LineString<point_type> linestring_type;
          linestring_type _lineString(_ring.begin(),_ring.end());
          
          if (intersects(
              _lineString,
              linestring_type(_polygon.boundary().begin(),_polygon.boundary().end())))
          {
            return false;
          }
          for (auto& _hole : _polygon.holes())
          {
            if (intersects(
                  _lineString,
                  linestring_type(_hole.begin(),_hole.end())))
            {
              return false;
            }    
          }
          return true;
        }
      }

      template<typename PRIMITIVE>
      struct Offset
        {};
    }
  }
}
