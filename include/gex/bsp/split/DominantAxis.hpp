#pragma once

#include "gex/base/Axis.hpp"

namespace gex
{
  namespace bsp
  {
    namespace split
    {
      struct DominantAxis
      {
        typedef base::Axis axis_type;

        template <typename STATE>
        axis_type operator()(const STATE& _state) const
        {
          return _state.nodeGeometry().bounds().dominantAxis();
        }
      };
    }
  }
}
