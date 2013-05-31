#pragma once

#include "DrawInterface.hpp"
#include "svg/Draw.hpp"
#include "svg/Buffer.hpp"
#include "svg/Ring.hpp"
#include "svg/Polygon.hpp"
#include "svg/LineString.hpp"
#include "svg/Point.hpp"
#include "svg/Bounds.hpp"
#include "svg/Vertex.hpp"
#include "svg/Segment.hpp"
#include "svg/BSP.hpp"

namespace gex
{
  namespace io
  {
    typedef DrawInterface<svg::Draw, svg::Buffer> SVG;
  }
}
