#pragma once

#include "ForEachRing.hpp"
#include "gex/algorithm/converter/LineString.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace functor
      {
        template<typename PRIMITIVE>
        struct ForEachLineString
        {
          typedef PRIMITIVE primitive_type;
          typedef typename primitive_type::model_type model_type;

          template<typename FUNCTOR>
          void operator()(primitive_type& _p, FUNCTOR _f) const
          {
            ForEachRing<PRIMITIVE>()(_p,[&](Ring& _ring) 
            {
              _f(algorithm::converter::convert<prim::LineString<model_type>>(_ring));
            });
          }

          template<typename FUNCTOR>
          void operator()(const primitive_type& _p, FUNCTOR _f) const
          {
            ForEachRing<PRIMITIVE>()(_p,[&](const Ring& _ring) 
            {
              _f(algorithm::converter::convert<prim::LineString<model_type>>(_ring));
            });
          }
        };

        template<typename MODEL>
        struct ForEachLineString< prim::LineString<MODEL> >
        {
          typedef prim::LineString<MODEL> primitive_type;

          template<typename FUNCTOR>
          void operator()(primitive_type& _p, FUNCTOR _f) const
          {
            _f(_p);
          }

          template<typename FUNCTOR>
          void operator()(const primitive_type& _p, FUNCTOR _f) const
          {
            _f(_p);
          }
        };

        template<typename MODEL>
        struct ForEachLineString< prim::Segment<MODEL> >
        {
          typedef prim::Segment<MODEL> primitive_type;

          template<typename FUNCTOR>
          void operator()(primitive_type& _p, FUNCTOR _f) const
          {
            _f(algorithm::converter::convert<prim::LineString<MODEL>>(_p));
          }

          template<typename FUNCTOR>
          void operator()(const primitive_type& _p, FUNCTOR _f) const
          {
            _f(algorithm::converter::convert<prim::LineString<MODEL>>(_p));
          }
        };

        template<typename PRIMITIVE>
        struct ForEachLineString< std::vector<PRIMITIVE> >
        {
          typedef PRIMITIVE primitive_type; 
          template<typename FUNCTOR>
          void operator()(primitive_type& _p, FUNCTOR _f) const
          {
            for (auto& p : _p) ForEachLineString<primitive_type>()(p,_f);
          }

          template<typename FUNCTOR>
          void operator()(const primitive_type& _p, FUNCTOR _f) const
          {
            for (const auto& p : _p) 
              ForEachLineString<primitive_type>()(p,_f);
          }        
        };
      }
    }
  }
