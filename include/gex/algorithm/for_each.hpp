#pragma once

#include "functor/ForEach.hpp"
#include "functor/ForEachRing.hpp"
#include "functor/ForEachPoint.hpp"
#include "functor/ForEachPointPair.hpp"
#include "functor/ForEachSegment.hpp"
#include "functor/ForEachMesh.hpp"

namespace gex
{
  namespace algorithm
  {
    using functor::ForEach;

    template<typename SUB_PRIMITIVE, typename PRIMITIVE, typename FUNCTOR>
    void for_each(const PRIMITIVE& _primitive, FUNCTOR f)
    {
      ForEach<SUB_PRIMITIVE,PRIMITIVE,true>()(_primitive,f);
    }

    template<typename SUB_PRIMITIVE, typename PRIMITIVE, typename FUNCTOR>
    void for_each(PRIMITIVE& _primitive, FUNCTOR f)
    {
      ForEach<SUB_PRIMITIVE,PRIMITIVE,false>()(_primitive,f);
    }
  }

  using algorithm::for_each;
}
