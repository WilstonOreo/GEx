#pragma once

#include "nearest/Generic.hpp"
#include "nearest/NearestContainer.hpp"
#include "nearest/KNearestContainer.hpp"
#include "nearest/InRadiusContainer.hpp"

namespace gex
{
    namespace algorithm
    {
      template<typename PRIMITIVE>
      using Nearest = nearest::Generic<nearest::NearestContainer<PRIMITIVE>>;
      
      template<typename PRIMITIVE>
      using KNearest = nearest::Generic<nearest::KNearestContainer<PRIMITIVE>>;

      template<typename PRIMITIVE>
      using InRadius = nearest::Generic<nearest::InRadiusContainer<PRIMITIVE>>;
    }
  }
