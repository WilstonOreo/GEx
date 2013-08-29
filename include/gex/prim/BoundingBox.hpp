#pragma once

#include "gex/base/Model.hpp"
#include "gex/base/Bounds.hpp"

namespace gex
{
    namespace prim
    {
      /** @brief A bounding is primitive which defines bounds of a compound object
       */
      template<class MODEL> 
      struct BoundingBox
      {
        typedef MODEL model_type;
        typedef base::Bounds<model_type> bounds_type;
        typedef base::Point<model_type> point_type;
        typedef base::Vec<model_type> vec_type;

        BoundingBox() : bounds_(bounds_type()) {}
        BoundingBox(point_type _min, point_type _max) : 
          bounds_(bounds_type(_min,_max)) 
        {
        }

        TBD_PROPERTY_REF(bounds_type,bounds)
      };
    }
  }
