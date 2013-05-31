#pragma once

#include "IntersectResult.hpp"
#include "gex/base.hpp"

namespace gex
{
    namespace bsp
    {
      namespace node
      {
        /**@brief A Geometry of node describes bounds, split position and split axis of a kd node
          *@detail Required for primitive intersection with a node
          */
        template <typename MODEL>
        struct Geometry : MODEL
        {
          GEX_MODEL(MODEL)
          typedef Axis axis_type;
          typedef base::Bounds<MODEL> bounds_type;
          typedef scalar_type splitpos_type;

          Geometry() {}
          Geometry(const bounds_type& _bounds,
                   const axis_type _axis,
                   const scalar_type _splitPos) :
            bounds_(_bounds),
            axis_(_axis),
            splitPos_(_splitPos)
          {}

          TBD_PROPERTY_REF(bounds_type,bounds)
          TBD_PROPERTY(axis_type,axis)
          TBD_PROPERTY(scalar_type,splitPos)
        };
      }
    }
  }
}
