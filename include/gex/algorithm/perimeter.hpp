#pragma once

#include <boost/geometry/algorithms/perimeter.hpp>
#include "for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Perimeter
      {
        template<typename PERIMETER>
        void operator()(const PRIMITIVE& _prim, PERIMETER& _perimeter)
        {
          _perimeter = boost::geometry::perimeter(_prim);
        }
      };
      
      template<typename POINT>
      struct Perimeter<gex::prim::Segment<POINT>>
      {
        typedef gex::prim::Segment<POINT> primitive_type;

        template<typename PERIMETER>
        void operator()(const primitive_type& _seg, PERIMETER& _perimeter)
        {
          _perimeter = (_seg.p1() - _seg.p0()).length(); 
        }
      };

      template<typename POINT>
      struct Perimeter<gex::prim::LineString<POINT>>
      {
        typedef gex::prim::LineString<POINT> primitive_type;

        template<typename PERIMETER>
        void operator()(const primitive_type& _prim, PERIMETER& _perimeter)
        {
          typedef gex::prim::Segment<POINT> segment_type;
          _perimeter = 0.0;
          for_each<segment_type>(_prim,[&](const segment_type& _seg)
          {
            PERIMETER _segLength;
            Perimeter<segment_type>()(_seg,_segLength);
            _perimeter += _segLength; 
          });
        }
      };
    }

    using functor::Perimeter;

    template<typename PRIMITIVE, typename PERIMETER>
    void perimeter(const PRIMITIVE& _prim, PERIMETER& _perimeter)
    {
      Perimeter<PRIMITIVE>()(_prim,_perimeter);
    }

    template<typename PRIMITIVE>
    typename PRIMITIVE::scalar_type perimeter(const PRIMITIVE& _prim)
    {
      typename PRIMITIVE::scalar_type _perimeter;
      perimeter(_prim,_perimeter);
      return _perimeter;
    }
  }

  using algorithm::perimeter;
}
