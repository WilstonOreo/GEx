#pragma once

#include "functor/BoundingSegments.hpp"

namespace gex
{
  namespace algorithm
  {
    using functor::BoundingSegments;

    template<typename SEGMENT>
    void boundingSegments(
      const typename SEGMENT::bounds_type& _bounds,
      const typename SEGMENT::scalar_type& _angle,
      std::pair<SEGMENT,SEGMENT>& _result)
    {
      BoundingSegments()(_bounds,_angle,_result);
    }

    template<typename SEGMENT>
    std::pair<SEGMENT,SEGMENT> boundingSegments(
      const typename SEGMENT::bounds_type& _bounds,
      const typename SEGMENT::scalar_type& _angle)
    {
      std::pair<SEGMENT,SEGMENT> _result;
      BoundingSegments()(_bounds,_angle,_result);
      return _result;
    }
  }
}
