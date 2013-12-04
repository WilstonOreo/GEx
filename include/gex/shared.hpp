#pragma once

#include "base.hpp"
#include "prim.hpp"

namespace gex
{
  namespace shared
  {
    /// Offset algorithms
    void offset(const Ring&, const Scalar&, MultiPolygon&);
    MultiPolygon offset(const Ring&, const Scalar&);
    
    void offset(const Polygon&, const Scalar&, MultiPolygon&);
    MultiPolygon offset(const Polygon&, const Scalar&);
    
    void offset(const MultiPolygon&, const Scalar&, MultiPolygon&);
    MultiPolygon offset(const MultiPolygon&, const Scalar&);
    
    /// Medial axis algorithms
    void medial_axis(const Polygon&, const Scalar&, MultiSegment&);
    MultiSegment medial_axis(const Polygon&, const Scalar&);

  }
}
