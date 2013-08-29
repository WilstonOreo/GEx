#pragma once
#include <boost/geometry/algorithms/centroid.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Centroid
      {
        template<typename POINT>
        void operator()(const PRIMITIVE& _prim, POINT& _point)
        {
          return boost::geometry::centroid(_prim,_point);
        }
      };
    }
    using functor::Centroid;

    template<typename PRIMITIVE, typename POINT>
    void centroid(const PRIMITIVE& _prim, POINT& _point)
    {
      Centroid<PRIMITIVE>()(_prim,_point);
    }

    template<typename PRIMITIVE>
    typename PRIMITIVE::point_type centroid(const PRIMITIVE& _prim)
    {
      typename PRIMITIVE::point_type _p;
      centroid(_prim,_p);
      return _p;
    }
  }

  using algorithm::centroid;
}
