#include <gex/algorithm/util.hpp>

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
          typedef typename point_type::scalar_type scalar_type;
          typedef typename point_type::model_type model_type;
          typedef base::Vec<model_type> vec_type;

          auto _arcFrac = std::abs(_a1 - _a0) * util::invPi2();
          int _steps = (int)(_arcFrac * M_PI / std::acos(1.0 - _limit / std::abs(_radius)));
          if (_steps < 2) _steps = 2;
          else if (_steps > (int)(222.0 * _arcFrac)) _steps = (int)(222.0 * _arcFrac);

          base::Vec2d _v;
          double _c, _s;
          sinCos(_a0,_v.y(),_v.x());
          sinCos((_a1 - _a0) / _steps, _s, _c);

          _out.reserve(_out.size() + _steps+1);
          for (int _i = 0; _i <= _steps; ++_i)
          {
            _out.emplace_back(_point.x() + _v.x()*_radius,_point.y() + _v.y()*_radius);
            auto _x2 = _v.x();
            _v(_v.x() * _c - _s * _v.y(), // cross product
               _x2 * _s + _v.y() * _c ); // dot product
          }
        }
      }
    }
  }
}


