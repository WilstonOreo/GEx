#pragma once

#include <gex/polygon.hpp>
#include "offset.hpp"
#include "medial_axis.hpp"

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
        typedef polygon_type output_type;

        template<typename IN, typename MIN_DETAIL, typename OFFSET, typename OUT>
        void operator()(const IN& _in, const MIN_DETAIL& _minDetail, const OFFSET& _offset, OUT& _out)
        {
          if (_offset < 0.0) return;
          auto&& _medial_axis = medial_axis(_in,gex::strategy::medial_axis::ScaleAxis(_minDetail));
          auto&& _offsetMedialAxis = offset(_medial_axis,_offset);
          
          polygon::MultiPolygon _multiPolygon;
          auto&& _bounds = _in.bounds();
          using namespace boost::polygon::operators;
          for (auto& _polygon : _offsetMedialAxis)
          {
            _polygon.update();
            _multiPolygon += polygon::adapt(_polygon,_bounds);
          }
          _multiPolygon += polygon::adapt(_in,_bounds);
          auto&& _outPolygons = polygon::adapt(_multiPolygon,_bounds);
          if (_outPolygons.size() != 1) return;
          _out = _outPolygons[0];
          _out.update();
        }
      };

      template<typename RING>
      struct Thicken<prim::Polygon<RING>> : Thicken<RING> 
      {
        typedef prim::Polygon<RING> output_type;
      };   
    }

    using functor::Thicken;

    template<typename IN, typename MIN_DETAIL, typename OFFSET, typename OUT>
    void thicken(const IN& _in, const MIN_DETAIL& _minDetail, const OFFSET& _offset, OUT& _out)
    {
      Thicken<IN>()(_in,_minDetail,_offset,_out);
    }
    
    template<typename IN, typename MIN_DETAIL, typename OFFSET>
    typename Thicken<IN>::output_type thicken(const IN& _in, const MIN_DETAIL& _minDetail, const OFFSET& _offset)
    {
      typename Thicken<IN>::output_type _out;
      thicken(_in,_minDetail,_offset,_out);
      return _out;
    }
  }

  using algorithm::thicken;
}
