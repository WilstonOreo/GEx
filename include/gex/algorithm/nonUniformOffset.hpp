#pragma once

#include "functor/Offset.hpp"


namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename T>
      struct NonUniformOffset {};

      template<typename POINT>
      struct NonUniformOffset<prim::Segment<POINT>>
      {
        typedef POINT point_type;
        typedef prim::Segment<point_type> segment_type;
        typedef std::pair<Scalar,Scalar> offset_type;
        typedef prim::Ring<point_type> result_type;
        
        void operator()(
            const segment_type& _seg, 
            const offset_type& _offset,
            result_type& _ring)
        {
          _ring.clear();
          auto _d = (_seg.p1() - _seg.p0()).normalized();
          Scalar _angle = atan2(_d.y(),_d.x());

          Scalar _pi2 = M_PI / 2.0;
          detail::buildArc(_seg.p0(),_angle+_pi2,_angle+3*_pi2,_offset.first,0.001,_ring);
          detail::buildArc(_seg.p1(),_angle-_pi2,_angle+_pi2,_offset.second,0.001,_ring);
          _ring.update();
        }
      };
    }

    using functor::NonUniformOffset;

    template<typename T, typename OFFSET, typename OUT>
    void nonUniformOffset(const T& _t, const OFFSET& _offset, OUT& _out)
    {
      NonUniformOffset<T>()(_t,_offset,_out);
    }

    template<typename T, typename OFFSET>
    typename NonUniformOffset<T>::result_type nonUniformOffset(const T& _t, const OFFSET& _offset)
    {
      typename NonUniformOffset<T>::result_type _out;
      nonUniformOffset(_t,_offset,_out);
      return _out;
    }
  }

  using algorithm::nonUniformOffset;
}


