#pragma once

#include "circle.hpp"

namespace create
{
  using gex::Polygon;

  /// create demo circle
  Polygon circleWith3Holes(
    const gex::Point2& _center,
    gex::Scalar _radius,
    int _numSegments = 48)
  {
    Polygon _polygon(circle(_center,_radius,false,_numSegments));
    _polygon.holes().push_back(circle(_center + Point2(-_radius/3,-_radius/3),_radius/4,true,_numSegments));
    _polygon.holes().push_back(circle(_center + Point2(_radius/3,-_radius/3),_radius/4,true,_numSegments));
    _polygon.holes().push_back(circle(_center + Point2(0,_radius/3),_radius/4,true,_numSegments));
    _polygon.update();
    return _polygon;
  }
}
