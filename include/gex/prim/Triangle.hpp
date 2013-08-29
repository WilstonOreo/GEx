#pragma once

#include "gex/base/Vec.hpp"
#include "gex/base/Bounds.hpp"

namespace gex
{
  namespace prim
  {
    template<class POINT>
    struct Triangle
    {
      typedef POINT point_type;
      typedef typename point_type::model_type model_type; 
      typedef typename point_type::scalar_type scalar_type;
      typedef base::Vec<model_type> vec_type;
      typedef base::Bounds<model_type> bounds_type;
      typedef std::array<point_type,3> points_type;

      Triangle() {}
      Triangle(const point_type& _v0,
               const point_type& _v1,
               const point_type& _v2,
               const vec_type& _n = vec_type())
      {
        points_[0] = _v0;
        points_[1] = _v1;
        points_[2] = _v2;
        normal(_n);
      }

      const point_type& v0() const
      {
        return points_[0];
      }
      const point_type& v1() const
      {
        return points_[1];
      }

      const point_type& v2() const
      {
        return points_[2];
      }

      void normal(const vec_type& _n)
      {
        normal_ = (_n.sqrLength() != 0) ? _n :
                  cross(points_[1] - points_[0],points_[2] - points_[0]);
      }

      TBD_PROPERTY_REF(points_type,points)
      TBD_PROPERTY_REF_RO(vec_type,normal)
    };
  }
}
