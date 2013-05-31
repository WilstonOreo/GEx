#pragma once

#include "functor/ForEachRing.hpp"
#include "functor/ForEachLineString.hpp"

namespace gex
{
  namespace algorithm
    {
      using boost::geometry::for_each_segment;
      using boost::geometry::for_each_point;

      using functor::ForEachRing;

      template<typename PRIMITIVE, typename FUNCTOR>
      void for_each_ring(const PRIMITIVE& _primitive, FUNCTOR f)
      {
        ForEachRing<PRIMITIVE>()(_primitive,f);
      }

      template<typename PRIMITIVE, typename FUNCTOR>
      void for_each_ring(PRIMITIVE& _primitive, FUNCTOR f)
      {
        ForEachRing<PRIMITIVE>()(_primitive,f);
      }

      using functor::ForEachLineString;

      template<typename PRIMITIVE, typename FUNCTOR>
      void for_each_linestring(const PRIMITIVE& _primitive, FUNCTOR f)
      {
        ForEachLineString<PRIMITIVE>()(_primitive,f);
      }

      template<typename PRIMITIVE, typename FUNCTOR>
      void for_each_linestring(PRIMITIVE& _primitive, FUNCTOR f)
      {
        ForEachLineString<PRIMITIVE>()(_primitive,f);
      }
    }
  }

