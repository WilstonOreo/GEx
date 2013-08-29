#pragma once

#include "markerFromPoint.hpp"
#include "chop.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE, typename SHIFT_PARAM>
      struct Shift {};
      
      template<typename POINT>
      struct Shift<prim::Ring<POINT>,typename POINT::scalar_type>
      {
        typedef prim::Ring<POINT> primitive_type;
        typedef typename POINT::scalar_type shift_type;
      
        void operator()(const primitive_type& _in, const shift_type& _shift, primitive_type& _out)
        {
          auto&& _lineStrings = chop(_in,std::vector<shift_type>({_shift}));
          if (_lineStrings.empty()) return;
          _out = primitive_type(_lineStrings.front().begin(),_lineStrings.front().end());
        }
      };
      
      template<typename POINT>
      struct Shift<prim::Ring<POINT>,POINT>
      {
        typedef prim::Ring<POINT> primitive_type;
        typedef POINT shift_type;
      
        void operator()(const primitive_type& _in, const shift_type& _shift, primitive_type& _out)
        {
          auto&& _marker = markerFromPoint(_in,_shift);
          auto&& _lineStrings = chop(_in,std::vector<typename primitive_type::scalar_type>({_marker}));
          if (_lineStrings.empty()) return;
          _out = primitive_type(_lineStrings.front().begin(),_lineStrings.front().end());
        }
      };

      template<typename RING, typename SHIFT>
      struct Shift<prim::Polygon<RING>,SHIFT>
      {
        typedef prim::Polygon<RING> primitive_type;

        void operator()(const primitive_type& _in, const SHIFT& _shift, primitive_type& _out)
        {
          Shift<RING,SHIFT>()(_in.boundary(),_shift,_out.boundary());
          for (auto& _hole : _in.holes())
          {
            RING _ring;
            Shift<RING,SHIFT>()(_hole,_shift,_ring);
            _out.holes().emplace_back(_ring);
          }
        }
      };

      template<typename POLYGON, typename SHIFT>
      struct Shift<prim::MultiPolygon<POLYGON>,SHIFT>
      {
        typedef prim::MultiPolygon<POLYGON> primitive_type;

        void operator()(const primitive_type& _in, const SHIFT& _shift, primitive_type& _out) 
        {
          _out.clear();
          for_each<POLYGON>(_in,[&](const POLYGON& _polygon) 
          {
            POLYGON _p;
            Shift<POLYGON,SHIFT>()(_polygon,_shift,_p);
            _out.emplace_back(_p);
          });
        }
      };
    }

    using functor::Shift;

    template<typename PRIMITIVE, typename SHIFT>
    void shift(const PRIMITIVE& _in, const SHIFT& _shift, PRIMITIVE& _out)
    {
      Shift<PRIMITIVE,SHIFT>()(_in,_shift,_out);
    }

    template<typename PRIMITIVE, typename SHIFT>
    PRIMITIVE shift(const PRIMITIVE& _in, const SHIFT& _shift)
    {
      PRIMITIVE _out;
      shift(_in,_shift,_out);
      return _out;
    }
  }

  using algorithm::shift;
}


