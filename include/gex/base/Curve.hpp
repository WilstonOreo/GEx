#pragma once

#include <gex/algorithm/util.hpp>

namespace gex
{
  namespace base
  {
    template<typename POINT>
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
        scalar_type _cos = (p2_ - p1_).normalized().dot((p0_ - p1_).normalized());
        return rad2deg(fabs(M_PI - acos(_cos)));
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
  }
}

