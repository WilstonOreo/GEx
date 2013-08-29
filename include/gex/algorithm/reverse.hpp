#pragma once

#include <gex/prim.hpp>
#include <boost/geometry/algorithms/reverse.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Reverse
      {
        void operator()(PRIMITIVE& _primitive)
        {
          boost::geometry::reverse(_primitive);
        }
      };

      template<typename POINT>
      struct Reverse<prim::LineString<POINT>>
      {
        void operator()(prim::LineString<POINT>& _ls)
        {
          std::reverse(_ls.begin(),_ls.end());
        }
      };

      template<typename POINT>
      struct Reverse<prim::Ring<POINT>>
      {
        void operator()(prim::Ring<POINT>& _r)
        {
          std::reverse(_r.begin(),_r.end());
        }
      };

      template<typename POINT>
      struct Reverse<prim::Segment<POINT>>
      {
        void operator()(prim::Segment<POINT>& _seg)
        {
          std::swap(_seg.front(),_seg.back());
        }
      };
    }
    using functor::Reverse;

    template<typename PRIMITIVE>
    void reverse(PRIMITIVE& _primitive)
    {
      Reverse<PRIMITIVE>()(_primitive);
    }
  }

  using algorithm::reverse;
}

