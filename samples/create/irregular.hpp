#pragma once

#include "circle.hpp"

namespace create
{
  /// create demo circle
  Ring irregular(
    Point2 _center,
    Scalar _radius,
    bool _inverse = false,
    int _numSegments = 48)
  {
    Ring _ring;
    for (int i = 0; i <= _numSegments; i++)
    {
      Scalar _angle = Scalar(i)/Scalar(_numSegments)*M_PI*2.0;
      Scalar _sin = _radius*sin(_angle),
             _cos = _radius*cos(_angle);

      if (i < _numSegments/4 || (i > _numSegments/2 && i <= 3*_numSegments/4))
      {
        _sin *= 0.5;
        _cos *= 0.5;
      }

      if (i == 0)
      {
        _sin *= 0.1;
        _cos *= 0.1;
      }

      if (_inverse) _sin = -_sin;
      _ring.push_back(Point2(_cos,_sin) + _center);
    }
    _ring.update();
    return _ring;
  }
}
