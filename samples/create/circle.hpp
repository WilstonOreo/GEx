#pragma once

namespace create
{
  using gex::Ring;
  using gex::Point2;
  using gex::Scalar;

  /// create demo circle
  Ring circle(const Point2& _center,
              Scalar _radius,
              bool _inverse = false,
              int _numSegments = 48)
  {
    Ring _ring(_inverse ? Ring::INNER : Ring::OUTER);
    for (int i = 1; i <= _numSegments; i++)
    {
      Scalar _angle = Scalar(i)/Scalar(_numSegments)*M_PI*2.0;
      Scalar _sin = _radius*sin(_angle),
             _cos = _radius*cos(_angle);

      if (_inverse) _sin = -_sin;
      _ring.push_back(Point2(_cos,_sin) + _center);
    }
    _ring.update();
    return _ring;
  }
}
