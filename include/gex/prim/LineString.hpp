#pragma once

#include <boost/geometry/algorithms/for_each.hpp>
#include <gex/algorithm/distance.hpp>

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
    struct LineString : std::vector<POINT>
    {
      /// Orientation of ring (CW = clockwise, CCW = counter-clockwise)
      enum Orientation { CW, CCW };

      typedef POINT point_type;
      typedef typename point_type::scalar_type scalar_type;
      typedef typename point_type::model_type model_type;
      typedef base::Range<scalar_type> range_type;
      typedef base::Bounds<model_type> bounds_type;
      typedef std::vector<point_type> ctnr_type;

      bool open(scalar_type _eps = 0.0) const 
      { 
        if (this->empty()) return false; 
        return sqrDistance(this->front(),this->back()) > _eps;
      }
    };

    template<class POINT>
    using MultiLineString = std::vector<LineString<POINT>>;
  }
}
