#pragma once

#include <tbd/add_const_if.h>
#include "gex/prim/Ring.hpp"
#include "gex/prim/Polygon.hpp"
#include "gex/prim/MultiPolygon.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace functor
      {
      template<typename SUB_PRIMITIVE, bool IS_CONST>
      struct ForEach
      {
      };

#define GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)\
      template<typename T>\
      using RefType = typename tbd::AddConstIf<T,IS_CONST>::ref_type;\
      typedef MODEL model_type;\
      typedef base::Point<model_type> point_type;\
      typedef RefType<point_type> point_ref_type;\
      typedef std::vector<point_type> multipoint_type;\
      typedef RefType<multipoint_type> multipoint_ref_type;\
      typedef prim::Segment<model_type> segment_type;\
      typedef RefType<segment_type> segment_ref_type;\
      typedef prim::LineString<model_type> linestring_type;\
      typedef RefType<linestring_type> linestring_ref_type;\
      typedef prim::MultiLineString<model_type> multilinestring_type;\
      typedef RefType<multilinestring_type> multilinestring_ref_type;\
      typedef prim::Ring<model_type> ring_type;\
      typedef RefType<ring_type> ring_ref_type;\
      typedef prim::Polygon<model_type> polygon_type;\
      typedef RefType<polygon_type> polygon_ref_type;\
      typedef prim::MultiPolygon<model_type> multipolygon_type;\
      typedef RefType<multipolygon_type> multipolygon_ref_type;

      /////////////////////////////////////////////////////////////
      // ForEach Point functor
      template<typename MODEL, bool IS_CONST>
      struct ForEach<base::Point<MODEL>,IS_CONST>
      {
        typedef base::Point<MODEL> sub_primitive_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)
        
        template<typename FUNCTOR>
        void operator()(point_ref_type _point, FUNCTOR f)
        {
          f(_point);
        }

        template<typename BEGIN, typename END, typename FUNCTOR> 
        void operator()(BEGIN _begin, END _end, FUNCTOR f)
        {
          for (auto it = _begin; it != _end; ++it)
            f(*it);
        }
        template<typename FUNCTOR>
        void operator()(multipoint_ref_type _multipoint, FUNCTOR f)
        {
          this->operator()(_multipoint.begin(),_multipoint.end(),f);
        }

        template<typename FUNCTOR>
        void operator()(segment_ref_type _segment, FUNCTOR f)
        {
          this->operator()(_segment[0],f); 
          this->operator()(_segment[1],f); 
        }

        template<typename FUNCTOR>
        void operator()(linestring_ref_type _lineString, FUNCTOR f)
        {
          this->operator()(_lineString.begin(),_lineString.end(),f);
        }

        template<typename FUNCTOR>
        void operator()(multilinestring_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString) this->operator()(_lineString,f);
        }
        
        template<typename FUNCTOR>
        void operator()(ring_ref_type _ring, FUNCTOR f)
        {
          this->operator()(_ring.begin(),_ring.end(),f);
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

      /////////////////////////////////////////////////////////////
      // ForEach PointPair
      template<typename MODEL, bool IS_CONST>
      struct ForEach<std::pair<base::Point<MODEL>,base::Point<MODEL>>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)
       
        template<typename BEGIN, typename END, typename FUNCTOR> 
        void operator()(BEGIN _begin, END _end, FUNCTOR f)
        {
          if (_end - _begin <= 1) return;

          auto it = _begin;
          while (it != _end-1)
          {
            auto& _p0 = *it;
            ++it;
            auto& _p1 = *it;
            f(_p0,_p1);
          }
        }

        template<typename FUNCTOR>
        void operator()(linestring_ref_type _lineString, FUNCTOR f)
        {
          this->operator()(_lineString.begin(),_lineString.end(),f);
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
          if (_ring.size() < 2) return;
          this->operator()(_ring.begin(),_ring.end(),f);
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

      /////////////////////////////////////////////////////////////
      // ForEach Segment functor
      template<typename MODEL, bool IS_CONST>
      struct ForEach<prim::Segment<MODEL>,IS_CONST>
      {
        typedef prim::Segment<MODEL> sub_primitive_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)
         
        template<typename FUNCTOR>
        void operator()(segment_ref_type _segment, FUNCTOR f)
        {
          f(_segment); 
        }

        template<typename BEGIN, typename END, typename FUNCTOR> 
        void operator()(BEGIN _begin, END _end, FUNCTOR f)
        {
          auto it = _begin;
          while (it != _end-1)
          {
            auto _p0 = *it;
            ++it;
            auto _p1 = *it;
            f(segment_type(_p0,_p1));
          }  
        }

        template<typename FUNCTOR>
        void operator()(linestring_ref_type _lineString, FUNCTOR f)
        {
          this->operator()(_lineString.begin(),_lineString.end(),f);
        }

        template<typename FUNCTOR>
        void operator()(multilinestring_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString) this->operator()(_lineString,f);
        }
        
        template<typename FUNCTOR>
        void operator()(ring_ref_type _ring, FUNCTOR f)
        {
          this->operator()(_ring.begin(),_ring.end(),f);
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

      /////////////////////////////////////////////////////////////
      // ForEach Ring functor
      template<typename MODEL, bool IS_CONST>
      struct ForEach<prim::Ring<MODEL>,IS_CONST>
      {
        typedef prim::Ring<MODEL> sub_primitive_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)
         
        template<typename FUNCTOR>
        void operator()(ring_ref_type _ring, FUNCTOR f)
        {
          f(_ring);
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

      /////////////////////////////////////////////////////////////
      // ForEach LineString functor
      template<typename MODEL, bool IS_CONST>
      struct ForEach<prim::LineString<MODEL>,IS_CONST>
      {
        typedef prim::LineString<MODEL> sub_primitive_type;
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)

        template<typename FUNCTOR>
        void operator()(linestring_ref_type _lineString, FUNCTOR f)
        {
          f(_lineString);
        }

        template<typename FUNCTOR>
        void operator()(multilinestring_ref_type _multiLineString, FUNCTOR f)
        {
          for (auto& _lineString : _multiLineString) this->operator()(_lineString,f);
        }
        
        template<typename FUNCTOR>
        void operator()(ring_ref_type _ring, FUNCTOR f)
        {
          linestring_type _lineString;
          _lineString.insert(_lineString.end(),_ring.begin(),_ring.end());
          this->operator()(_lineString,f);
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

