#pragma once

#include "gex/base/Bounds.hpp"

namespace gex
{
    namespace prim
    {
      template<typename SCALAR>
      struct Sphere 
      {
        typedef SCALAR scalar_type;
        typedef base::Model<3,scalar_type> model_type;
        typedef base::Point<model_type> point_type;
        typedef base::Vec<model_type> vec_type;
        typedef base::Bounds<model_type> bounds_type;

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
