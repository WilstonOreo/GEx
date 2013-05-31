#pragma once

#include "Primitive.hpp"
#include "gex/base/Model.hpp"
#include "gex/base/Bounds.hpp"

namespace gex
{
    namespace prim
    {
      /** @brief A bounding is primitive which defines bounds of a compound object
       */
      template<class MODEL> 
      struct BoundingBox :
        Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)

        BoundingBox() : bounds_(bounds_type()) {}
        BoundingBox(point_type _min, point_type _max) : 
          bounds_(bounds_type(_min,_max)) 
        {
        }

        TBD_PROPERTY_REF(bounds_type,bounds)
      };
    }
  }
