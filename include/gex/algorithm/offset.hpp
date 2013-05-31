#pragma once

#include "functor/Offset.hpp"

namespace gex
{
    namespace algorithm
    {
      using functor::Offset;

      template<typename PRIMITIVE>
      void offset(const PRIMITIVE& _in, MultiPolygon& _out, Scalar _offset)
      {
        Offset<PRIMITIVE>()(_in,_out,_offset);
      }

      template<typename PRIMITIVE>
      MultiPolygon offset(const PRIMITIVE& _in, Scalar _offset)
      {
        MultiPolygon _out;
        Offset<PRIMITIVE>()(_in,_out,_offset);
        return _out;
      }
    }
  }
