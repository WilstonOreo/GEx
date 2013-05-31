#pragma once

#include "Polygon.hpp"

namespace gex
{
    namespace prim
    {
      template<typename MODEL>
      struct MultiPolygon :
          Primitive<MODEL>,
          std::vector<Polygon<MODEL>>
       {
        GEX_PRIMITIVE(MODEL)

        typedef std::vector<Polygon<MODEL>> ctnr_type;
        typedef typename ctnr_type::size_type size_type;

        MultiPolygon() :
          correct_(false)
        {
        }
        /// Universal constructor based on assign()
        template<class T> MultiPolygon(const T& _t) :
          correct_(false)
        {
          assign(_t.begin(),_t.end());
        }

        /// universal assign
        template<class INSERT_ITERATOR>
        void assign(INSERT_ITERATOR _from, INSERT_ITERATOR _to)
        {
          correct_ = false;
          assign(_from,_to);
        }

        void push_back(const Polygon<MODEL>& _polygon)
        {
          correct_ = false;
          ctnr_type::push_back(_polygon);
        }

        void update()
        {
          if (correct_) return;
          bounds_type _bounds;
          for ( auto& _polygon : *this )
          {
            _polygon.update();
            _bounds.extend(_polygon.bounds());
          }
          boost::geometry::correct(*this);
          bounds_=_bounds;
        }

        const size_type countPoints()
        {
          size_type _count=0;
          for ( auto& _p : *this)
            _count += _p.points();
          return _count;
        }

        TBD_PROPERTY_RO(bool,correct)
        TBD_PROPERTY_REF_RO(bounds_type,bounds)
      };
    }
  }
