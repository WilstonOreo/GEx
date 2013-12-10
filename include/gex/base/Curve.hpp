#pragma once

#include <gex/algorithm/util.hpp>

namespace gex
{
  namespace base
  {
    template<typename POINT, bool IS_CONST = true>
    struct Curve
    {
      typedef POINT point_type;
      typedef typename POINT::Scalar scalar_type;

      Curve(const POINT& _p0, const POINT& _p1, const POINT& _p2) :
        p0_(_p0),
        p1_(_p1),
        p2_(_p2) {}

      scalar_type determinant() const
      {
        return algorithm::util::determinant(p0_,p1_,p2_);
      }

      scalar_type angle() const
      {
        auto _u = p2_ - p1_;
        auto _v = p0_ - p1_;

        scalar_type _cos = _u.normalized().dot(_v.normalized());
        if (_cos <= -1.0) return 0.0;  
        if (_cos >= 1.0) return 180.0;  
        scalar_type _theta = rad2deg(fabs(acos(-_cos)));
        if (_theta != _theta) return 0.0;
        return _theta;
      }
     /* 
      scalar_type angle() const
      {
        return (p2_ - p1_).normalized().dot((p0_ - p1_).normalized());
      }*/

      point_type const& p0() const { return p0_; }
      point_type const& p1() const { return p1_; }
      point_type const& p2() const { return p2_; }

    private:
      point_type const& p0_;
      point_type const& p1_;
      point_type const& p2_;
    };

    template<typename POINT>
    struct Curve<POINT,false>
    {
      typedef POINT point_type;
      typedef typename POINT::Scalar scalar_type;

      Curve(point_type& _p0, point_type& _p1, point_type& _p2) :
        p0_(_p0),
        p1_(_p1),
        p2_(_p2) {}

      scalar_type determinant() const
      {
        return algorithm::util::determinant(p0_,p1_,p2_);
      }

      scalar_type angle() const
      {
        auto _u = p2_ - p1_;
        auto _v = p0_ - p1_;

        scalar_type _cos = _u.normalized().dot(_v.normalized());
        if (_cos <= -1.0) return 0.0;  
        if (_cos >= 1.0) return 180.0;  
        scalar_type _theta = rad2deg(fabs(acos(-_cos)));
        if (_theta != _theta) return 0.0;
        return _theta;
      }
      
      point_type& p0() { return p0_; }
      point_type& p1() { return p1_; }
      point_type& p2() { return p2_; }
      point_type const& p0() const { return p0_; }
      point_type const& p1() const { return p1_; }
      point_type const& p2() const { return p2_; }

    private:
      point_type& p0_;
      point_type& p1_;
      point_type& p2_;
    };
  }
}

