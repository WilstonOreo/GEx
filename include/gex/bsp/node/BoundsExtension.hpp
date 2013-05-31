#pragma once

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      template<typename PRIMITIVE>
      struct BoundsExtension
      {
        template<typename BOUNDS>
        void operator()(
          const PRIMITIVE& _primitive,
          BOUNDS& _bounds)
        {
          _bounds.extend(_primitive.bounds());
        }
      };
    }
  }
}
