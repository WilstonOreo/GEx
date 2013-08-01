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

      template<typename MODEL>
      struct Num<base::Point<MODEL>,prim::Segment<MODEL>>
      {
        typedef prim::Segment<MODEL> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return 2;
        }
      };

      template<typename MODEL, typename PRIMITIVE>
      struct Num<base::Point<MODEL>,PRIMITIVE>
      {
        typedef PRIMITIVE primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return _prim.size();
        }
      };
      
      template<typename SUB_PRIMITIVE, typename MODEL>
      struct Num<SUB_PRIMITIVE,prim::MultiLineString<MODEL>>
      {
        typedef prim::MultiLineString<MODEL> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          size_t _num = 0;
          for (auto& _p : _prim)
            _num += Num<SUB_PRIMITIVE,prim::LineString<MODEL>>(_p); 
          return _num;
        }
      };

      template<typename SUB_PRIMITIVE, typename MODEL>
      struct Num<SUB_PRIMITIVE,prim::Polygon<MODEL>>
      {
        typedef prim::Polygon<MODEL> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          typedef prim::Ring<MODEL> ring_type;
          size_t _num = 0;
          for_each<SUB_PRIMITIVE>(_prim,[&](const ring_type& _ring)
          {
            _num += Num<SUB_PRIMITIVE,ring_type>()(_ring);
          });
          return _num;
        }
      };
      
      template<typename SUB_PRIMITIVE, typename MODEL>
      struct Num<SUB_PRIMITIVE,prim::MultiPolygon<MODEL>>
      {
        typedef prim::MultiPolygon<MODEL> primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          size_t _num = 0;
          typedef prim::Ring<MODEL> ring_type;
          for_each<ring_type>(_prim,[&](const ring_type& _r)
          {
            _num += Num<SUB_PRIMITIVE,ring_type>()(_r);
          });
          return _num;
        }
      };

      template<typename MODEL, typename PRIMITIVE>
      struct Num<prim::Segment<MODEL>,PRIMITIVE>
      {
        typedef PRIMITIVE primitive_type;
        size_t operator()(const primitive_type& _prim)
        {
          return Num<base::Point<MODEL>,primitive_type>(_prim)-1;
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
}
