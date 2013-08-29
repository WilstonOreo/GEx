#pragma once

#include "offset.hpp"
#include "skeleton.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Thicken {};

      template<typename POINT>
      struct Thicken<prim::Ring<POINT>>
      {
        typedef prim::Ring<POINT> ring_type;
        typedef prim::Polygon<ring_type> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;

        template<typename IN, typename OFFSET, typename OUT>
        void operator()(const IN& _in, const OFFSET& _offset, OUT& _out)
        {
          if (_offset < 0.0) return;
          auto&& _skeleton = skeleton(_in);
          auto&& _offsetSkeleton = offset(_skeleton,_offset);
          for (auto& _polygon : _offsetSkeleton) 
            _polygon.update();
          _offsetSkeleton = unify(_offsetSkeleton);
          union_(_offsetSkeleton,_in,_out);
        }
      };

      template<typename RING>
      struct Thicken<prim::Polygon<RING>> : Thicken<RING> 
      {
        typedef prim::Polygon<RING> polygon_type;
        typedef prim::MultiPolygon<polygon_type> result_type;
      };
      
      template<typename POLYGON>
      struct Thicken<prim::MultiPolygon<POLYGON>> : Thicken<POLYGON> 
      {
        typedef prim::MultiPolygon<POLYGON> result_type;
      };
    }

    using functor::Thicken;

    template<typename IN, typename OFFSET, typename OUT>
    void thicken(const IN& _in, const OFFSET& _offset, OUT& _out)
    {
      Thicken<IN>()(_in,_offset,_out);
    }
    
    template<typename IN, typename OFFSET>
    typename Thicken<IN>::result_type thicken(const IN& _in, const OFFSET& _offset)
    {
      typename Thicken<IN>::result_type _out;
      thicken(_in,_offset,_out);
      return _out;
    }
  }

  using algorithm::thicken;
}
