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
        auto&& _v0 = (p2_ - p1_).normalized();
        auto&& _n = algorithm::util::getNormal(p1_,p0_).normalized();
        return dot(_n,_v0);
      }

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

