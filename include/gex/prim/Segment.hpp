#pragma once

#include <array>
#include "Primitive.hpp"

namespace gex
{
    namespace prim
    {
      template<typename MODEL> 
        struct Segment : public Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)
        
          Segment() {}
        Segment(const point_type& _p0, const point_type& _p1)
        {
          p_[0] = _p0;
          p_[1] = _p1;
        }

        vec_type normal() const
        {
          vec_type _d = p_[1] - p_[0];
          return vec_type(-_d.y(),_d.x());
        }

        bounds_type bounds() const
        {
          bounds_type _bounds;

          _bounds.extend(p_[0]);
          _bounds.extend(p_[1]);
          return _bounds;
        }

        /// Methods to access coordinate value in a certain dimension
        point_type& operator[] (size_t i)
        {
          GEX_ASSERT(i < 2);
          return p_[i];
        }

        const point_type& operator[] (size_t i) const
        {
          GEX_ASSERT(i < 2);
          return p_[i];
        }

        point_type& p0()
        {
          return p_[0];
        }

        const point_type& p0() const
        {
          return p_[0];
        }

        point_type& p1()
        {
          return p_[1];
        }

        const point_type& p1() const
        {
          return p_[1];
        }

        void operator()(const point_type& _p0,
                        const point_type& _p1)
        {
          p_[0] = _p0; 
          p_[1] = _p1;
        }


        void swap() 
        {
          std::swap(p_[0],p_[1]);
        }
        template<class ARCHIVE>
        void serialize( ARCHIVE& _ar, const unsigned int _fileVersion )
        {
          _ar & p_[0];
          _ar & p_[1];
        }

      private:
        std::array<point_type,2> p_;
      };
    }
  }


