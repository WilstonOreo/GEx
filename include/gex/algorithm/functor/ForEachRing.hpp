#pragma once

#include "gex/prim/Ring.hpp"
#include "gex/prim/Polygon.hpp"
#include "gex/prim/MultiPolygon.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace functor
      {
        /// Template concept for letting a functor perform on each ring of a certain primitve
        template<typename PRIMITIVE>
        struct ForEachRing
        {

        };

        template<typename MODEL>
        struct ForEachRing < prim::Ring<MODEL>>
        {
          typedef prim::Ring<MODEL> primitive_type;

          template<typename FUNCTOR>
          void operator()(primitive_type& _ring, FUNCTOR _f) const
          {
            _f(_ring);
          }
        };

        /// Template concept for letting a functor perform on a std::vector of rings
        template<typename MODEL>
        struct ForEachRing< std::vector<prim::Ring<MODEL>> >
        {
          typedef prim::Ring<MODEL> Ring;
          typedef std::vector<Ring> primitive_type;

          template<typename FUNCTOR>
          void operator()(primitive_type& _rings, FUNCTOR _f) const
          {
            for ( auto& _ring : _rings) _f(_ring);
          }

          template<typename FUNCTOR>
          void operator()(const primitive_type& _rings, FUNCTOR _f) const
          {
            for ( const auto& _ring : _rings) _f(_ring);
          }
        };

        /// Template concept for letting a functor perform on the rings of a polygon
        template<typename MODEL>
        struct ForEachRing<prim::Polygon<MODEL>>
        {
          typedef prim::Polygon<MODEL> primitive_type;

          template<typename FUNCTOR>
          void operator()(const primitive_type& _polygon, FUNCTOR _f) const
          {
            _f(_polygon.boundary());
            ForEachRing<std::vector<prim::Ring<MODEL>>>()(_polygon.holes(),_f);
          }

          template<typename FUNCTOR>
          void operator()(primitive_type& _polygon, FUNCTOR _f) const
          {
            _f(_polygon.boundary());
            ForEachRing<std::vector<prim::Ring<MODEL>>>()(_polygon.holes(),_f);
          }
        };

        /// Template concept for letting a functor perform on the rings of a multipolygon
        template<typename MODEL>
        struct ForEachRing<prim::MultiPolygon<MODEL>>
        {
          typedef prim::Polygon<MODEL> Polygon;
          typedef prim::MultiPolygon<MODEL> primitive_type;

          template<typename FUNCTOR>
          void operator()(const primitive_type& _multiPolygon, FUNCTOR _f) const
          {
            for ( const auto& _polygon : _multiPolygon )
              ForEachRing<prim::Polygon<MODEL>>()(_polygon,_f);
          }

          template<typename FUNCTOR>
          void operator()(primitive_type& _multiPolygon, FUNCTOR _f) const
          {
            for ( auto& _polygon : _multiPolygon )
              ForEachRing<prim::Polygon<MODEL>>()(_polygon,_f);
          }
        };
      }
    }
  }
