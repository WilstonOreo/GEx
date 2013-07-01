#pragma once

#include "gex/prim.hpp"
#include "ForEachRing.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct ForEachPoint
      {
        template<typename FUNCTOR>
        void operator()(PRIMITIVE& _primitive, FUNCTOR f)
        {
          for (auto& _p : _primitive) 
            f(_p);
        }
        
        template<typename FUNCTOR>
        void operator()(const PRIMITIVE& _primitive, FUNCTOR f)
        {
          for (auto& _p : _primitive) 
            f(_p);
        }
      };

      template<typename MODEL>
      struct ForEachPoint<prim::Segment<MODEL>> 
      {
        template<typename FUNCTOR>
        void operator()(prim::Segment<MODEL>& _s, FUNCTOR f)
        {
          f(_s[0]); f(_s[1]);
        }
        
        template<typename FUNCTOR>
        void operator()(const prim::Segment<MODEL>& _s, FUNCTOR f)
        {
          f(_s[0]); f(_s[1]);
        }
      };
      
      template<typename MODEL>
      struct ForEachPoint<prim::Polygon<MODEL>> 
      {
        typedef prim::Polygon<MODEL> polygon_type;
        typedef prim::Ring<MODEL> ring_type;

        template<typename FUNCTOR>
        void operator()(polygon_type& _p, FUNCTOR f)
        {
          ForEachRing<polygon_type>()(_p,[&](ring_type& _r)
          {
            ForEachPoint<ring_type>()(_r,f);
          });
        }
        
        template<typename FUNCTOR>
        void operator()(const polygon_type& _p, FUNCTOR f)
        {
          ForEachRing<polygon_type>()(_p,[&](const ring_type& _r)
          {
            ForEachPoint<ring_type>()(_r,f);
          });
        }
      };

      template<typename MODEL>
      struct ForEachPoint<prim::MultiPolygon<MODEL>> 
      {
        typedef prim::MultiPolygon<MODEL> polygon_type;
        typedef prim::Ring<MODEL> ring_type;

        template<typename FUNCTOR>
        void operator()(polygon_type& _p, FUNCTOR f)
        {
          ForEachRing<polygon_type>()(_p,[&](ring_type& _r)
          {
            ForEachPoint<ring_type>()(_r,f);
          });
        }
        
        template<typename FUNCTOR>
        void operator()(const polygon_type& _p, FUNCTOR f)
        {
          ForEachRing<polygon_type>()(_p,[&](const ring_type& _r)
          {
            ForEachPoint<ring_type>()(_r,f);
          });
        }
      };

      template<typename PRIMITIVE>
      struct ForEachPoint<std::vector<PRIMITIVE>> 
      {
        typedef std::vector<PRIMITIVE> primitives_type;

        template<typename FUNCTOR>
        void operator()(primitives_type& _primitives, FUNCTOR f)
        {
          for (auto& _p : _primitives) 
            ForEachPoint<PRIMITIVE>()(_p,f);
        }
        
        template<typename FUNCTOR>
        void operator()(const primitives_type& _primitives, FUNCTOR f)
        {
          for (auto& _p : _primitives) 
            ForEachPoint<PRIMITIVE>()(_p,f);
        } 
      };
    }
  }
}
