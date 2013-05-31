#pragma once

#include <vector>

namespace gex
{
  namespace bsp
  {
    namespace container
    {
      template<typename PRIMITIVE>
      using PrimitiveContainer = std::vector<PRIMITIVE>;

      template<typename PRIMITIVE>
      using PrimitivePtrContainer = std::vector<const PRIMITIVE*>;
    }
  }
}

