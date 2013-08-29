#pragma once

#include "converter/Segment.hpp"
#include "converter/Bounds.hpp"
#include "converter/LineString.hpp"
#include "converter/Polygon.hpp"

namespace gex
{
  namespace algorithm
  {
    using converter::Converter;
    using converter::convert;
  }

  using algorithm::convert;
}
