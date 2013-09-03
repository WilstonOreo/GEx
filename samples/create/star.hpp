#pragma once

#include "circle.hpp"

namespace create
{
  using gex::Ring;
  using gex::Point2;
  using gex::Scalar;

  /// create demo circle
  Ring star(const Point2& _center,
            Scalar _radiusInner,
            Scalar _radiusOuter,
            bool _inverse = false,
            int _numSpikes = 48)
  {
    Ring _ring(_inverse ? Ring::INNER : Ring::OUTER);

    if (_radiusInner > _radiusOuter) std::swap(_radiusInner,_radiusOuter);

    for (int i = 1; i <= _numSpikes*2; i++)
    {
      auto _radius = (i % 2) ? _radiusInner : _radiusOuter;
      Scalar _angle = Scalar(i)/Scalar(_numSpikes*2)*M_PI*2.0;
      Scalar _sin = _radius*sin(_angle),
             _cos = _radius*cos(_angle);
      if (_inverse) _sin = -_sin;
      _ring.push_back(Point2(_cos,_sin) + _center);
    }
    _ring.update();
    return _ring;
  }

}
