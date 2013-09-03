#pragma once

#include <gex/prim.hpp>
#include "for_each.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename SUB_PRIMITIVE, typename PRIMITIVE>
      struct Num 
      {
        size_t operator()(const PRIMITIVE& _primitive)
        {
          return std::is_same<PRIMITIVE,SUB_PRIMITIVE>::value;
        }
      };

      template<typename POINT>
      struct Num<POINT,prim::Segment<POINT>>
      {
        typedef prim::Segment<POINT> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return 2;
        }
      };

      template<typename POINT>
      struct Num<POINT,prim::LineString<POINT>>
      {
        template<typename PRIM>
        size_t operator()(const PRIM& _prim)
        {
          return _prim.size();
        }
      };

      template<typename POINT>
      struct Num<POINT,prim::Ring<POINT>> : 
        Num<POINT,prim::LineString<POINT>> {};

      
      template<typename SUB_PRIMITIVE, typename POINT>
      struct Num<SUB_PRIMITIVE,prim::MultiLineString<POINT>>
      {
        typedef prim::MultiLineString<POINT> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          size_t _num = 0;
          for (auto& _p : _prim)
            _num += Num<SUB_PRIMITIVE,prim::LineString<POINT>>(_p); 
          return _num;
        }
      };

      template<typename SUB_PRIMITIVE, typename RING>
      struct Num<SUB_PRIMITIVE,prim::Polygon<RING>>
      {
        typedef prim::Polygon<RING> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          size_t _num = 0;
          for_each<SUB_PRIMITIVE>(_prim,[&](const RING& _ring)
          {
            _num += Num<SUB_PRIMITIVE,RING>()(_ring);
          });
          return _num;
        }
      };
      
      template<typename SUB_PRIMITIVE, typename POLYGON>
      struct Num<SUB_PRIMITIVE,prim::MultiPolygon<POLYGON>>
      {
        typedef prim::MultiPolygon<POLYGON> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          size_t _num = 0;
          typedef typename POLYGON::ring_type ring_type;
          for_each<ring_type>(_prim,[&](const ring_type& _r)
          {
            _num += Num<SUB_PRIMITIVE,ring_type>()(_r);
          });
          return _num;
        }
      };

      template<typename POINT, typename PRIMITIVE>
      struct Num<prim::Segment<POINT>,PRIMITIVE>
      {
        typedef PRIMITIVE primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return Num<POINT,primitive_type>(_prim)-1;
        }
      };

      template<typename MODEL>
      struct Num<prim::Ring<MODEL>,prim::Polygon<MODEL>>
      {
        typedef prim::Polygon<MODEL> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return _prim.holes().size() + 1;
        }
      };
    }
    
    using functor::Num;

    template<typename SUB_PRIMITIVE, typename PRIMITIVE>
    size_t num(const PRIMITIVE& _primitive)
    {
      return Num<SUB_PRIMITIVE,PRIMITIVE>()(_primitive);
    } 
  }
  using algorithm::num;
}
