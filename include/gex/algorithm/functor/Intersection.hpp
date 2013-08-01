namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINT, typename PRECISION = double>
        bool operator()(
          const POINT& _a,
          const POINT& _b,
          const POINT& _c,
          const POINT& _d,
          POINT& _iPoint,
          PRECISION _eps = 0.0001)
        {
          typedef POINT point_type;
          typedef typename POINT::scalar_type scalar_type;

          PRECISION x1 = _a.x(), x2 = _b.x(), x3 = _c.x(), x4 = _d.x();
          PRECISION y1 = _a.y(), y2 = _b.y(), y3 = _c.y(), y4 = _d.y();
          PRECISION d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

          // If d is zero, there is no intersection
          if (std::abs(d) <= _eps) return false;

          // Get the x and y
          PRECISION pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
          PRECISION x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
          PRECISION y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

          // Check if the x and y coordinates are within both lines
          if ( x < std::min(x1, x2) || x > std::max(x1, x2) ||
               x < std::min(x3, x4) || x > std::max(x3, x4) ) return false;
          if ( y < std::min(y1, y2) || y > std::max(y1, y2) ||
               y < std::min(y3, y4) || y > std::max(y3, y4) ) return false;

          _iPoint(x,y);

          return true;
        }
      }
    }
  }
}
