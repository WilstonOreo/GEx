#pragma once

#include <array>
#include "gex/base/Bounds.hpp"

namespace gex
{
  namespace prim
  {
    template<typename POINT>
    struct Segment
    {
      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Vec<model_type> vec_type;
      typedef base::Bounds<model_type> bounds_type;

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

      point_type& front() { return p0(); }
      point_type& back() { return p1(); }
      point_type const& front() const { return p0(); }
      point_type const& back() const { return p1(); }

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

    private:
      std::array<point_type,2> p_;
    };

    template<typename POINT>
    struct MultiSegment : std::vector<Segment<POINT>>
    {
      typedef Segment<POINT> primitive_type;
      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Range<scalar_type> range_type;
      typedef std::vector<point_type> ctnr_type;
      typedef base::Bounds<model_type> bounds_type;
    };
  }
}
