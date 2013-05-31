#pragma once

#include "Generic.hpp"
#include "gex/prim.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      template<>
      struct Converter<Bounds2,std::vector<Segment>>
      {
      };

      template<>
      struct Converter<Bounds2,LineString>
      {
      };

      template<>
      struct Converter<Bounds2,MultiLineString>
      {
      };

      template<>
      struct Converter<Bounds2,std::vector<Point2>>
      {
      };
    }
  }
}
