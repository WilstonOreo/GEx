#pragma once

#include "ForEach.hpp"
#include <gex/base/Curve.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      /// For each curve in linestring
      template<typename POINT, bool IS_CONST>
      struct ForEach<base::Curve<POINT>,prim::LineString<POINT>,IS_CONST>
      {
        typedef base::Curve<POINT> curve_type;
        typedef prim::LineString<POINT> primitive_type;

        template<typename FUNCTOR>
        void operator()(const primitive_type& _lineString, FUNCTOR f)
        {
          size_t _i = 0, _n = _lineString.size()-2;
          while (_i < _n)
          {
            auto& _p0 = _lineString[_i];
            auto& _p1 = _lineString[_i+1];
            auto& _p2 = _lineString[_i+2];
            f(curve_type(_p0,_p1,_p2));
            ++_i;
          }
        }
      };

      /// For each curve in ring
      template<typename POINT, bool IS_CONST>
      struct ForEach<base::Curve<POINT>,prim::Ring<POINT>,IS_CONST>
      {
        typedef base::Curve<POINT> curve_type;
        typedef prim::Ring<POINT> primitive_type;

        template<typename FUNCTOR>
        void operator()(const primitive_type& _ring, FUNCTOR f)
        {
        
          size_t _i = 0, _n = _ring.size();
          if (_n < 3) return;
          while (_i < _n-3)
          {
            auto& _p0 = _ring[_i];
            auto& _p1 = _ring[_i+1];
            auto& _p2 = _ring[_i+2];
            f(curve_type(_p0,_p1,_p2));
            ++_i;
          }
          f(curve_type(_ring[_n-3],_ring[_n-2],_ring[0]));
          f(curve_type(_ring[_n-2],_ring[0],_ring[1]));
        }
      };

      /// ForEach point in curve
      template<typename POINT, bool IS_CONST>
      struct ForEach<POINT,base::Curve<POINT>,IS_CONST>
      {
        typedef base::Curve<POINT> curve_type;

        template<typename CURVE, typename FUNCTOR>
        void operator()(const CURVE& _curve, FUNCTOR f)
        {
          f(_curve.p0());
          f(_curve.p1());
          f(_curve.p2());
        }
      };
    }
  }
}

