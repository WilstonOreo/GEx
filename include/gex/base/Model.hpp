#pragma once

#include "gex/misc.hpp"

namespace gex
{
    namespace base
    {
      template<size_t DIMENSIONS, typename SCALAR>
      struct Model
      {
        typedef SCALAR scalar_type;

        static constexpr size_t dimensions() { return DIMENSIONS; }

        /// Maximum corner
        static const scalar_type scalarMax()
        {
          return std::numeric_limits<scalar_type>::max();
        }
        /// Minimum corner
        static const scalar_type scalarMin()
        {
          return std::numeric_limits<scalar_type>::min();
        }
      };
    }
  }

#define GEX_MODEL(MODEL) \
      typedef MODEL model_type; \
      using typename MODEL::scalar_type; \
      using MODEL::dimensions;

