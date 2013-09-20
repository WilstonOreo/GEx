#pragma once

#include "for_each.hpp"
#include "perimeter.hpp"
#include "distance.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE, bool IS_CONST = true>
      struct Step
      {
      };

      template<typename POINT, bool IS_CONST>
      struct Step<prim::Segment<POINT>,IS_CONST>
      {
        typedef typename tbd::AddConstIf<prim::Segment<POINT>,IS_CONST>::ref_type segment_ref_type; 

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
            segment_ref_type _segment, 
            const MARKERS& _markers, 
            SEGMENT_FUNCTOR sf,
            POINT_FUNCTOR pf)
        {
          pf(_segment.p0());
          if (!_markers.empty()) 
          {
            auto&& _v = _segment.p1() - _segment.p0();
            for (auto& _marker : _markers)
            {
              if (_marker < 0 || _marker >= 1) continue;
              auto&& _p = _segment.p0() + _v * _marker;
              sf(_p);
            }
          }
        }
      };

      namespace 
      {
        template<
          typename POINT,        
          typename BEGIN,
          typename END,
          typename LENGTH,
          typename MARKERS,
          typename SEGMENT_FUNCTOR,
          typename POINT_FUNCTOR>
        void step_range(
          BEGIN _begin, END _end,
          const LENGTH& _length,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          typedef prim::Segment<POINT> segment_type;
          auto&& _invLength = 1.0 / _length;
          MARKERS _segmentMarkers;
          Scalar _d = 0;

          typedef std::pair<POINT,POINT> pointpair_type;
          for_each_point_pair_range(_begin,_end,[&](const POINT& _p0, const POINT& _p1)
          {
            auto&& _segmentLength = distance(_p0,_p1);
            auto&& _length = _segmentLength * _invLength;
            _segmentMarkers.clear();
            auto _lower = std::lower_bound(_markers.begin(),_markers.end(),_d),
                 _upper = std::lower_bound(_markers.begin(),_markers.end(),_d+_length);
            for (auto it = _lower ; it != _upper; ++it)
            {
              auto _marker = (*it - _d) / _length; 
              _segmentMarkers.push_back(_marker);
            }
            Step<segment_type>()(segment_type(_p0,_p1),_segmentMarkers,sf,pf);
            _d += _length;
          });
        }
      }



      template<typename POINT, bool IS_CONST>
      struct Step<prim::LineString<POINT>,IS_CONST>
      {
        typedef typename tbd::AddConstIf<prim::LineString<POINT>,IS_CONST>::ref_type linestring_ref_type; 

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          linestring_ref_type _ls,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          step_range<POINT>(_ls.begin(),_ls.end(),perimeter(_ls),_markers,sf,pf);
          sf(_ls.back());
        }
      };

      template<typename POINT, bool IS_CONST>
      struct Step<prim::Ring<POINT>,IS_CONST>
      {
        typedef typename tbd::AddConstIf<prim::Ring<POINT>,IS_CONST>::ref_type ring_ref_type; 

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          ring_ref_type _ring,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          step_range<POINT>(_ring.begin(),_ring.end(),boost::geometry::perimeter(_ring),_markers,sf,pf);
        }
      };
    }

    using functor::Step;

    template<typename PRIMITIVE, typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
    void step(const PRIMITIVE& _prim, const MARKERS& _markers, SEGMENT_FUNCTOR sf, POINT_FUNCTOR pf)
    {
      Step<PRIMITIVE>()(_prim,_markers,sf,pf);
    }
    
    template<typename PRIMITIVE, typename MARKERS, typename SEGMENT_FUNCTOR>
    void step(const PRIMITIVE& _prim, const MARKERS& _markers, SEGMENT_FUNCTOR sf)
    {
      Step<PRIMITIVE>()(_prim,_markers,sf,[&](const typename PRIMITIVE::point_type& _p){});
    }
  }
}

