#pragma once

#include <gex/algorithm/intersection.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct FillBounds
      {
        typedef PRIMITIVE primitive_type;
        typedef typename primitive_type::point_type point_type;
        typedef prim::MultiLineString<point_type> output_type;

        template<typename BOUNDS, typename PATTERN, typename GENERATOR>
        void operator()(const primitive_type& _primitive, const BOUNDS& _bounds, PATTERN& _pattern, GENERATOR _gen)
        {
          output_type _out;
          _gen(_bounds,_out);
          gex::algorithm::intersection(_primitive,_out,_pattern);
        }
      };

      template<typename PRIMITIVE>
      struct Fill
      {
        typedef PRIMITIVE primitive_type;
        typedef typename primitive_type::point_type point_type;
        typedef prim::MultiLineString<point_type> output_type;

        template<typename PATTERN, typename GENERATOR>
        void operator()(const primitive_type& _primitive, PATTERN& _pattern, GENERATOR _gen)
        {
          FillBounds<PRIMITIVE>()(_primitive,_primitive.bounds(),_pattern,_gen);
        }
      };
    }

    using functor::Fill;
    using functor::FillBounds;

    template<typename PRIMITIVE, typename PATTERN, typename GENERATOR>
    void fill(const PRIMITIVE& _primitive, PATTERN& _pattern, GENERATOR _gen)
    {
      Fill<PRIMITIVE>()(_primitive,_pattern,_gen);
    }

    template<typename PRIMITIVE, typename GENERATOR>

    typename GENERATOR::output_type fill(const PRIMITIVE& _primitive, GENERATOR _gen)
    {
      typename GENERATOR::output_type _pattern;
      fill(_primitive,_pattern,_gen);
      return _pattern;
    }
    
    template<typename PRIMITIVE, typename BOUNDS, typename PATTERN, typename GENERATOR>
    void fillBounds(const PRIMITIVE& _primitive, const BOUNDS& _bounds, PATTERN& _pattern, GENERATOR _gen)
    {
      FillBounds<PRIMITIVE>()(_primitive,_bounds,_pattern,_gen);
    }

    template<typename PRIMITIVE, typename BOUNDS, typename GENERATOR>
    typename GENERATOR::output_type fillBounds(const PRIMITIVE& _primitive, const BOUNDS& _bounds, GENERATOR _gen)
    {
      typename GENERATOR::output_type _pattern;
      fillBounds(_primitive,_bounds,_pattern,_gen);
      return _pattern;
    }
  }

  using algorithm::fill;
  using algorithm::fillBounds;
}


