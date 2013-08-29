#pragma once

#include "functor/SqrDistance.hpp"

namespace gex
{
  namespace algorithm
  {
    using functor::SqrDistance;

    template<typename A, typename B>
    typename A::scalar_type sqrDistance(const A& _a, const B& _b)
    {
      return SqrDistance<A,B>()(_a,_b);
    }
    
    template<typename A, typename B>
    typename A::scalar_type distance(const A& _a, const B& _b)
    {
      return std::sqrt(sqrDistance(_a,_b));
    }
  }

  using algorithm::distance;
  using algorithm::sqrDistance;
}
