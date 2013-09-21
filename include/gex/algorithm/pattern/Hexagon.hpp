#pragma once

#include <gex/algorithm/boundingSegments.hpp>
#include <gex/algorithm/reverse.hpp>
#include <gex/algorithm/util.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace pattern
    {
      static const gex::Scalar sin60_ = 0.5 * std::sqrt(3.0);

      template<typename POINT>
      struct Hexagon
      {
        typedef POINT point_type;
        typedef typename point_type::Scalar scalar_type;
        typedef prim::MultiLineString<point_type> output_type;

        Hexagon(
          scalar_type _angle,
          scalar_type _length,
          scalar_type _distance = 0.0) :
          angle_(_angle),
          length_(_length),
          distance_(_distance) {}

        template<typename BOUNDS>
        void operator()(const BOUNDS& _bounds, output_type& _output)
        {
          typedef prim::Segment<point_type> segment_type;
          std::pair<segment_type,segment_type> _boundingSegments;
          gex::algorithm::boundingSegments(_bounds,angle(),_boundingSegments);

          BOUNDS _angleBounds;
          _angleBounds.extend(_boundingSegments.first.bounds());
          _angleBounds.extend(_boundingSegments.second.bounds());

          float _d = (distance() + length())*sin60_;
          for (gex::Scalar _y = _angleBounds.min().y() ; _y < _angleBounds.max().y(); _y += _d * 2.0)
          {
            generateLine(_angleBounds,gex::Point2(_angleBounds.min().x(),_y),_output,false);
            generateLine(_angleBounds,gex::Point2(_angleBounds.min().x(),_y + distance() * sin60_),_output,true);
          }
        }

        TBD_PROPERTY_REF(scalar_type,angle)
        TBD_PROPERTY_REF(scalar_type,length)
        TBD_PROPERTY_REF(scalar_type,distance)

      private:
        template<typename BOUNDS, typename LINESTRINGS>
        void generateLine(
          const BOUNDS& _bounds,
          const POINT& _point,
          LINESTRINGS& _ls,
          bool _flip)
        {
          gex::LineString _line;
          _line.emplace_back(_point);

          auto _center = _bounds.center();

          int _mode = 0;

          while (_line.back().x() <= _bounds.max().x() &&
                 _line.back().y() <= _bounds.max().y())
          {
            gex::Vec2 _delta;
            if (_mode % 2 == 0)
            {
              _delta(0) = length_; 
              _delta(1) = 0;
              _line.push_back(_line.back() + _delta);
            }
            else
            {
              _delta(0) = length_*0.5;
              _delta(1) = length_ * sin60_;
              if (_flip)
                _delta.y() = -_delta.y();

              if (_mode % 4 == 1)
                _delta.y() = -_delta.y();

              gex::Point2 _p(_line.back() + _delta);

              _line.push_back(_p);
            }

            _mode++;
          }

          if (_flip)
            reverse(_line);

          for_each<POINT>(_line,[&](POINT& _point)
          {
            util::rotatePoint(_center,misc::deg2rad(angle()),_point);
          });

          _ls.push_back(_line);
        }

      };
    }
  }
}

