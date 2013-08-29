#pragma once

#include <boost/geometry/algorithms/envelope.hpp>
#include <gex/prim.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Envelope
      {
        template<typename ENVELOPE>
        void operator()(const PRIMITIVE& _prim, ENVELOPE& _envelope)
        {
          _envelope = boost::geometry::envelope(_prim);
        }
      };
      
      template<typename POINT>
      struct Envelope<gex::prim::Segment<POINT>>
      {
        typedef gex::prim::Segment<POINT> primitive_type;

        template<typename ENVELOPE>
        void operator()(const primitive_type& _seg, ENVELOPE& _envelope)
        {
          _envelope = _seg.bounds();
        }
      };

      template<typename POINT>
      struct Envelope<gex::prim::MultiPoint<POINT>>
      {
        typedef gex::prim::MultiPoint<POINT> primitive_type;

        template<typename ENVELOPE>
        void operator()(const primitive_type& _prim, ENVELOPE& _envelope)
        {
          _envelope = ENVELOPE();
          for (auto& _p : _prim)
          {
            _envelope.extend(_p);
          }
        }
      };
    }

    using functor::Envelope;
    
    template<typename PRIMITIVE, typename BOUNDS>
    void envelope(const PRIMITIVE& _prim, BOUNDS& _bounds)
    {
      Envelope<PRIMITIVE>()(_prim,_bounds);
    }

    template<typename PRIMITIVE>
    typename PRIMITIVE::bounds_type envelope(const PRIMITIVE& _prim)
    {
      typename PRIMITIVE::bounds_type _bounds;
      envelope(_prim,_bounds);
      return _bounds;
    }
  }
}
