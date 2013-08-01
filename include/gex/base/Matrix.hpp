#pragma once

#include "Coords.hpp"
#include <boost/numeric/ublas/matrix.hpp>

namespace gex
{
  namespace base
  {
#define GEX_FOREACH_COEFF(i,j) \
      for (size_t i = 0; i < this->size1(); ++i)\
        for (size_t j = 0; j < this->size2(); ++j)
 
    template<typename MODEL>
    using MatrixBase = boost::numeric::ublas::matrix<typename MODEL::scalar_type>;

    template<class MODEL>
    class Matrix : public MODEL, public MatrixBase<MODEL>
    {
    public:
      GEX_MODEL(MODEL)
      typedef Coords<MODEL> coords_type;

      Matrix() : MatrixBase<MODEL>(MODEL::dimensions(),MODEL::dimensions())
      {
        loadIdentity();
      }

      void loadIdentity()
      {
        GEX_FOREACH_COEFF(i,j)
        {
          this->operator()(i,j) = (i == j) ? 1 : 0;
        }
      }

      template<typename VEC>
      void translate(const VEC& _vec)
      {
        for (size_t j = 0; j < _vec.dimensions()-1; ++j) 
          this->operator()(_vec.dimensions()-1,j) += _vec[j];
      }

      template<typename POINT>
      void moveTo(const POINT& _p)
      {
        for (size_t j = 0; j < _p.dimensions()-1; ++j) 
          this->operator()(_p.dimensions()-1,j) = _p[j];
      }
    };
  }
}
