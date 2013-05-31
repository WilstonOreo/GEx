#pragma once

namespace gex
{
  namespace bsp
  {
    namespace split
    {
      template
      <
      int N_PRIMITIVES = 3
      >
      struct Half
      {
        template<typename BUILD_STATE, typename AXIS, typename SPLIT_POS>
        bool operator()(
          const BUILD_STATE& _state,
          const AXIS _axis,
          SPLIT_POS& _splitPos)
        {
          if (_state.primitives().size() < N_PRIMITIVES) return false;

          const auto& _bounds = _state.nodeGeometry().bounds();
          _splitPos = (_bounds.min()[_axis]+_bounds.max()[_axis])/2;
          return true;
        }
      };
    }
  }
}
