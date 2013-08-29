#pragma once

#include "ForEach.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      // ForEach PointArray functor
      template<typename MODEL, size_t N, bool IS_CONST>
      struct ForEach<std::array<base::Point<MODEL>,N>,IS_CONST>
      {
        typedef std::array<base::Point<MODEL>,N> sub_primitive_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)
        
        template<typename FUNCTOR>
        void operator()(linestring_ref_type _lineString, FUNCTOR f)
        {
          if (_lineString.size() < N) return;
          size_t _i = 0, _n = _lineString.size() - N;

          while (_i < _n)
          {
            sub_primitive_type _points;
            size_t _j = 0;
            for (auto& _p : _points)
            {
              _p = _lineString[_i + _j];
              ++_j;
            }
            f(_points);
            ++_i;
          }
        }

        template<typename FUNCTOR>
        void operator()(multilinestring_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString) 
            this->operator()(_lineString,f);
        }
        
        template<typename FUNCTOR>
        void operator()(ring_ref_type _ring, FUNCTOR f)
        {
          if (_ring.empty()) return;
          size_t _i = 0, _n = _ring.size() - 1;

          while (_i < _n)
          {
            sub_primitive_type _points;
            size_t _j = 0;
            for (auto& _p : _points)
            {
              _p = _ring[(_i + _j) % _n];
              ++_j;
            }
            f(_points);
            ++_i;
          }
        }

        template<typename FUNCTOR>
        void operator()(polygon_ref_type _polygon, FUNCTOR f)
        {
          this->operator()(_polygon.boundary(),f);
          for (auto& _ring : _polygon.holes()) this->operator()(_ring,f);
        }

        template<typename FUNCTOR>
        void operator()(multipolygon_ref_type _multiPolygon, FUNCTOR f)
        {
          this->operator()(_multiPolygon.boundary(),f);
          for (auto& _polygon : _multiPolygon.holes()) this->operator()(_polygon,f);
        }
      };


    }
  }
}
