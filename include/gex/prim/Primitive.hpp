#pragma once

#include "gex/base/Ray.hpp"
#include "gex/base/Bounds.hpp"

namespace gex
{
    namespace prim
    {
      /// A primitive is an object which has an extent and for which an intersection point can be found
      template<class MODEL>
      struct Primitive : MODEL
      {
        GEX_MODEL(MODEL)
        typedef base::Point<MODEL> point_type;
        typedef base::Vec<MODEL> vec_type;
        typedef base::Bounds<MODEL> bounds_type;
        typedef base::Ray<MODEL> ray_type;
        typedef base::Range<scalar_type> range_type;
      };
    }
  }

#define GEX_PRIMITIVE(MODEL) \
        typedef typename MODEL::scalar_type scalar_type;\
        using MODEL::dimensions;\
        typedef gex::prim::Primitive<MODEL> primitive_type; \
        using typename primitive_type::point_type; \
        using typename primitive_type::vec_type; \
        using typename primitive_type::bounds_type; \
        using typename primitive_type::ray_type; \
        using typename primitive_type::range_type;

