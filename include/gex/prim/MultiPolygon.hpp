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

        typedef Polygon<MODEL> polygon_type;
        typedef typename polygon_type::ring_type ring_type;
        typedef std::vector<polygon_type> container_type;
        typedef typename container_type::size_type size_type;

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

        using container_type::empty;
        using container_type::size;
        typedef typename container_type::iterator iterator;
        typedef typename container_type::const_iterator const_iterator;

        const_iterator begin() const
        {
          return container_type::begin();
        }

        const_iterator end() const
        {
          return container_type::end();
        }


        iterator begin()
        {
          correct_ = false;
          return container_type::begin();
        }

        iterator end()
        {
          correct_ = false;
          return container_type::end();
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
          container_type::push_back(_polygon);
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


        TBD_PROPERTY_RO(bool,correct)
        TBD_PROPERTY_REF_RO(bounds_type,bounds)
      };
    }
  }
