#pragma once

#include "nearest/Generic.hpp"
#include "nearest/NearestContainer.hpp"
#include "sqrDistance.hpp"

namespace gex
{
    namespace algorithm
    {
      template<typename PRIMITIVE, typename PRIMITIVE_SET = std::vector<PRIMITIVE>>
      using Nearest = nearest::Generic<nearest::NearestContainer<PRIMITIVE>,algorithm::SqrDistance<PRIMITIVE,PRIMITIVE>,PRIMITIVE_SET>;
    }
  }
