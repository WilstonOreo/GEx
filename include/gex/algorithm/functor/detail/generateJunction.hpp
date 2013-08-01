#include <gex/base.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINT, typename RING>
        void generateJunction( const POINT& _p0,
                               const POINT& _p1,
                               const POINT& _p2,
                               typename POINT::scalar_type _offset,
                               typename POINT::scalar_type _limit,
                               RING& _ring)
        {
          typedef POINT point_type;
          typedef typename POINT::scalar_type scalar_type;
          typedef typename RING::vec_type vec_type;

          _ring.clear();
          if ((_p0 - _p1).length() < _limit)
            return;

          // Normals
          gex::base::Vec2d _n0 = getNormal(_p0,_p1).normalized(),
                           _n1 = getNormal(_p1,_p2).normalized();

          // Scaled normals;
          vec_type _sn0(_n0); _sn0 *= _offset;
          vec_type _sn1(_n1); _sn1 *= _offset;

          _ring.clear();
          _ring.push_back(_p0 + _sn1);
          double _a0 = std::atan2(_n1.y(),_n1.x()),
                 _a1 = std::atan2(_n0.y(),_n0.x());

          if (_offset > 0 && _a1 < _a0) _a1 += M_PI * 2.0;
          else if (_offset < 0 && _a1 > _a0) _a1 -= M_PI * 2.0;

          if (_offset < 0)
          {
            std::swap(_a0,_a1);
          }

          if (_a1 - _a0 < M_PI)
          {
            if (_offset < 0) std::swap(_a0,_a1);
            detail::buildArc(_p1,_a1,_a0,_offset,_limit,_ring);
          }
          else
          {
            gex::Segment _seg0(_p0 + _sn0,
                               _p1 + _sn0),
                         _seg1(_p1 + _sn1,
                               _p2 + _sn1);
            gex::Point2 _iPoint;
            if (detail::lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            }
            else
            {
              _ring.clear();
              _ring.push_back(_seg0.p0());
              _ring.push_back(_seg0.p1());
              _ring.push_back(_seg1.p0());
              _ring.push_back(_seg1.p1());
              _ring.update();
              return;
            }
          }
          _ring.push_back(_p2 + _sn1);
          _ring.push_back(_p2);
          _ring.push_back(_p1);
          _ring.push_back(_p0);
          _ring.update();
        }
      }
    }
  }
}
