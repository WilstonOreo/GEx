#pragma once

#include <utility>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      struct BoundingSegments
      {
        template<typename SEGMENT>
        void operator()
        (const typename SEGMENT::bounds_type& _bounds, float _angle,
         std::pair<SEGMENT,SEGMENT>& _result)
        {
          typedef typename SEGMENT::scalar_type scalar_type;
          typedef typename SEGMENT::vec_type vec_type;
          typedef typename SEGMENT::point_type point_type;

          scalar_type _radius = _bounds.radius();
          scalar_type _cos = _radius * cos(_angle*M_PI/180),
                      _sin = _radius * sin(_angle*M_PI/180);
          vec_type _cross(-_sin,_cos);
          vec_type _dir(_cos,_sin);
          point_type _center = _bounds.center();

          _result.first[0] = _center - _cross - _dir;
          _result.first[1] = _result.first[0] + 2.0*_dir;
          _result.second[0] = _center + _cross - _dir;
          _result.second[1] = _result.second[0] + 2.0*_dir;
        }
      };
    }
  }
}
