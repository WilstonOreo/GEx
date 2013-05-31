#pragma once

#include "Primitive.hpp"

namespace gex
{
    namespace prim
    {
      template<typename MODEL>
      struct Sphere : Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)

        Sphere(scalar_type _radius = 1.0, 
               point_type _center = vec_type()) :
          radius_(_radius),
          center_(_center)
        {}

        vec_type normal(const point_type& _iPoint) const
        {
          return vec_type(_iPoint - center()).normalized();
        }

        TBD_PROPERTY_REF(scalar_type,radius)
        TBD_PROPERTY_REF(point_type,center)
      };
    }
  }
