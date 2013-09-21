#pragma once

#include <boost/geometry/algorithms/for_each.hpp>
#include <gex/algorithm/distance.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/length.hpp>
#include <gex/algorithm/boost.hpp>

namespace gex
{
  namespace prim
  {
    namespace detail
    {
      template<typename SEGMENT>
      struct Functor
      {
        Functor(std::vector<SEGMENT>& _segments) :
          segments_(_segments) {}

        template<class T> void operator()( const T& _segment)
        {
          segments_.push_back(SEGMENT(_segment.first,_segment.second));
        }
        std::vector<SEGMENT>& segments_;
      };
    }

    template<class POINT>
    struct LineString : base::VecContainer<POINT>
    {
      LineString() {}

      template<typename BEGIN, typename END>
      LineString(BEGIN _begin, END _end) 
      {
        this->reserve(_end - _begin);
        std::for_each(_begin,_end,[&](const point_type& _point)
        {
          this->push_back(_point);
        });
      }

      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Range<scalar_type> range_type;
      typedef base::Bounds<model_type> bounds_type;
      typedef base::VecContainer<point_type> ctnr_type;

      bool open(scalar_type _eps = 0.0) const 
      { 
        if (this->empty()) return false; 
        return sqrDistance(this->front(),this->back()) > _eps;
      }

      void push_back(const point_type& _point)
      {
        correct_ = false;
        ctnr_type::push_back(_point);
      }

      void update()
      {
        if (correct_) return;
        bounds_type _bounds;
        for (auto& _p : *this)
          _bounds.extend(_p);
        bounds_=_bounds;
        correct_ = true;
      }

      const bounds_type& bounds() const
      {
        return bounds_;
      }

      TBD_PROPERTY_RO(bool,correct)

    private:
      bounds_type bounds_;    
    };

    template<class POINT>
    using MultiLineString = std::vector<LineString<POINT>>;
  }
}
