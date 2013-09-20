#pragma once

#include <gex/misc.hpp>
#include "boost/geometry/extensions/algorithms/buffer/line_line_intersection.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace util
    {
      template<typename POINT>
      Vec2 getNormal(const POINT& _p0, const POINT& _p1)
      {
        auto&& _d = _p1 - _p0;
        return Vec2(-_d.y(),_d.x());
      }


      template<typename ORIGIN, typename ANGLE, typename POINT>
      void rotatePoint(const ORIGIN& _o, ANGLE _theta, POINT& _p)
      {
        auto _d = _p - _o;
        _p.x() = cos(_theta) * _d.x() - sin(_theta) * _d.y() + _o.x();
        _p.y() = sin(_theta) * _d.x() + cos(_theta) * _d.y() + _o.x();
      }


      static inline constexpr double invPi2()
      {
        return 1.0 / (2.0 * M_PI);
      }

      template<typename POINT>
      typename POINT::scalar_type determinant(
        const POINT& _a,
        const POINT& _b,
        const POINT& _c)
      {
        return (_b.x() * _c.y() + _a.x() * _b.y() + _a.y()*_c.x()) -
               (_a.y() * _b.x() + _b.y() * _c.x() + _a.x()*_c.y());
      }

      template<typename VALUE, typename RESULT>
      void sinCos(const VALUE& _val, RESULT& _sin, RESULT& _cos)
      {
        _sin = std::sin(_val);
        _cos = std::cos(_val);
      }

      /// Generic line segment intersection
      /// http://flassari.is/2008/11/line-line-intersection-in-cplusplus/
      template<typename POINT> 
      bool lineSegmentIntersection(
          const POINT& _p1, 
          const POINT& _p2, 
          const POINT& _p3, 
          const POINT& _p4, 
          POINT& _point)
      {
        typedef typename POINT::Scalar scalar_type;

        // Store the values for fast access and easy
        // equations-to-code conversion
        scalar_type x1 = _p1.x(), x2 = _p2.x(), x3 = _p3.x(), x4 = _p4.x();
        scalar_type y1 = _p1.y(), y2 = _p2.y(), y3 = _p3.y(), y4 = _p4.y();
        scalar_type d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        // If d is zero, there is no intersection
        if (d == 0) return false;
 
        // Get the x and y
        scalar_type pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);

        scalar_type x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
        scalar_type y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
        // Check if the x and y coordinates are within both lines
        if ( x < std::min(x1, x2) || x > std::max(x1, x2) ||
             x < std::min(x3, x4) || x > std::max(x3, x4) ) return false;
        if ( y < std::min(y1, y2) || y > std::max(y1, y2) ||
             y < std::min(y3, y4) || y > std::max(y3, y4) ) return false;
 
        // Return the point of intersection
        _point(0) = x; _point(1) = y;
        return true;
      }

      template<typename SEGMENT, typename POINT> 
      bool lineSegmentIntersection(
          const SEGMENT& _seg0, 
          const SEGMENT& _seg1,
          POINT& _point)
      {
        return lineSegmentIntersection(_seg0.p0(),_seg0.p1(),_seg1.p0(),_seg1.p1(),_point); 
      }
    }
  }
}
