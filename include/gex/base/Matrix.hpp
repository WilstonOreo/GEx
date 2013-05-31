#pragma once

#include "Coords.hpp"

namespace gex
{
  namespace base
  {
#define GEX_FOREACH_COEFF(i,j) \
      GEX_FOREACH_DIM(i) \
        GEX_FOREACH_DIM(j) \
 
    template<class MODEL>
    class Matrix : MODEL
    {
    public:
      GEX_MODEL(MODEL)
      typedef Coords<MODEL> coords_type;

      Matrix()
      {
        loadIdentity();
      }

      void loadIdentity()
      {
        GEX_FOREACH_COEFF(i,j)
        {
          Matrix::operator()(i,j) = (i == j) ? 1 : 0;
        }
      }

      const scalar_type& operator()(int _x, int _y) const
      {
        return a_[_y][_x];
      }

      scalar_type& operator()(int _x, int _y)
      {
        return a_[_y][_x];
      }

    private:
      coords_type a_[dimensions()];
    };
  }
}
