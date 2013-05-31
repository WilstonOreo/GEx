#pragma once

#include "functor/ConnectSegments.hpp"

namespace gex
{
  namespace algorithm
  {
    using functor::ConnectSegments;

    template<typename SEGMENTS, typename OUTPUT>
    void connectSegments(const SEGMENTS& _segments, OUTPUT& _output, double _epsilon = 0.00000001)
    {
      ConnectSegments()(_segments,_output,_epsilon);
    }

    template<typename OUTPUT, typename SEGMENTS>
    OUTPUT connectSegments(const SEGMENTS& _segments, double _epsilon = 0.00000001)
    {
      OUTPUT _output;
      ConnectSegments()(_segments,_output,_epsilon);
      return _output;
    }
  }
}
