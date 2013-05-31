#pragma once

#include <map>
#include "Color.hpp"

namespace gex
{
  namespace color
  {
    template<typename MODEL, typename CALC_T=float>
    struct Gradient :
        std::map<CALC_T,Color<MODEL>>
    {
    };
  }
}
