#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <tbd/property.h>
#include <tbd/parse_utils.h>
#include <limits>
#include <iostream>

#include "misc/assert.hpp"

namespace gex
{
  namespace misc
  {
    namespace defaults
    {
      typedef float ScalarType;
    }

    template<class T = defaults::ScalarType> T deg2rad( T _value )
    {
      return _value * (T)(M_PI / 180.0);
    }

    template<class T = defaults::ScalarType> T rad2deg( T _value )
    {
      return _value * (T)(180.0 / M_PI);
    }

    template<typename T = defaults::ScalarType>
    static const T inf()
    {
      return std::numeric_limits<T>::max();
    }

    template<typename T = defaults::ScalarType>
    static const T rnd()
    {
      return T(rand())/T(RAND_MAX);
    }
  }

  using misc::deg2rad;
  using misc::rad2deg;
}


