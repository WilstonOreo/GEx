#pragma once

namespace gex
{
  namespace strategy
  {
    namespace medial_axis
    {
      struct Joined {};
      struct SegmentsOnly {};

      struct ScaleAxis
      {
        ScaleAxis(float _s = 1.0) :
          s_(_s) {}
        TBD_PROPERTY_RO(float,s)
      };

      struct Pruning
      {
        Pruning(float _factor) :
          factor_(_factor) {}
        TBD_PROPERTY_RO(float,factor)
      };
    }
  }
}

