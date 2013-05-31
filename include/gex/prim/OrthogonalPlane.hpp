#pragma once

#include "Primitive.hpp"

namespace gex
{
    namespace prim
    {
      template<typename MODEL, base::Axis AXIS>
      struct OrthogonalPlane : Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)

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
