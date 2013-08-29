#pragma once

#include "gex/base/Vec.hpp"
#include "gex/base/Point.hpp"
#include "gex/base/Bounds.hpp"

namespace gex
{
  namespace prim
  {
    template<typename SCALAR, base::Axis AXIS>
    struct OrthogonalPlane
    {
      typedef SCALAR scalar_type;
      typedef base::Model<3,scalar_type> model_type;
      typedef base::Point<model_type> point_type;
      typedef base::Vec<model_type> vec_type;
      typedef base::Bounds<model_type> bounds_type;

      OrthogonalPlane() {}
      OrthogonalPlane(const scalar_type& _pos) : pos_(_pos) {}

      static constexpr base::Axis axis()
      {
        return AXIS;
      }

      vec_type normal() const
      {
        vec_type _normal;
        _normal[AXIS] = 1.0;
        return _normal;
      }

      TBD_PROPERTY_REF(scalar_type,pos)
    };
  }
}
