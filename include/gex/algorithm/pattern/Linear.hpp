#pragma once

#include <gex/algorithm/boundingSegments.hpp>
#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/reverse.hpp>


namespace gex
{
  namespace algorithm
  {
    namespace pattern
    {
      template<typename POINT>
      struct Linear
      {
        typedef POINT point_type;
        typedef typename point_type::Scalar scalar_type;
        typedef prim::MultiLineString<point_type> output_type;

        Linear(
          scalar_type _angle,
          scalar_type _distance) :
          angle_(_angle),
          distance_(_distance) {}

        template<typename BOUNDS>
        void operator()(const BOUNDS& _bounds, output_type& _output)
        {
          typedef prim::Segment<point_type> segment_type;
          std::pair<segment_type,segment_type> _boundingSegments;
          gex::algorithm::boundingSegments(_bounds,angle(),_boundingSegments);

          auto _fillingDirection = _boundingSegments.second[0] - _boundingSegments.first[0];

          unsigned _nLines = _fillingDirection.norm() / distance();
          auto _inc = _fillingDirection * (1.0 / _nLines);

          _output.reserve(_output.size()+_nLines*2);
          gex::Segment _seg(_boundingSegments.first[0] + 0.5*_inc,_boundingSegments.first[1] + 0.5*_inc);

          for (unsigned i = 1; i < _nLines; ++i)
          {
            _seg[0] += _inc;
            _seg[1] += _inc;
            reverse(_seg);
            _output.push_back(gex::convert<gex::prim::LineString<POINT>>(_seg));
          }
        }

        TBD_PROPERTY_REF(scalar_type,angle)
        TBD_PROPERTY_REF(scalar_type,distance)
      };
    }
  }
}

