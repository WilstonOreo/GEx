#pragma once

#include "skeleton.hpp"
#include "closestPoint.hpp"

namespace gex
{
  namespace algorithm
  {
    template<typename POLYGON, typename MULTIPOLYGON>
    gex::MultiLineString offsetDetails(const POLYGON& _polygon, const MULTIPOLYGON& _offsetPolygons, gex::Scalar _offset, gex::Scalar _minOffset)
    {
      auto&& _voronoi = gex::algorithm::voronoi(_polygon);
      gex::MultiSegment _segments;

      auto&& _skeleton = functor::detail::skeleton_remove_segments_from_voronoi(_polygon,_voronoi);

      for (auto& _segment : _skeleton)
      {
        if (!gex::within(_segment.p0(),_polygon)) continue;
        if (!gex::within(_segment.p1(),_polygon)) continue;
        if (gex::within(_segment.p0(),_offsetPolygons)) continue;
        if (gex::within(_segment.p1(),_offsetPolygons)) continue;

        auto&& _p0 = gex::closestPoint(_polygon,_segment.p0());
        auto&& _p1 = gex::closestPoint(_polygon,_segment.p1());
        auto&& _dist0 = gex::distance(_p0,_segment.p0());
        auto&& _dist1 = gex::distance(_p1,_segment.p1());
        if (_dist0 < _minOffset || _dist1 < _minOffset) continue;

        _p0 = gex::closestPoint(_offsetPolygons,_segment.p0());
        _p1 = gex::closestPoint(_offsetPolygons,_segment.p1());
        _dist0 = gex::distance(_p0,_segment.p0());
        _dist1 = gex::distance(_p1,_segment.p1());

        gex::Scalar _absOffset = std::max(std::abs(_offset),_minOffset);

        if (_dist0 < _absOffset && _dist1 < _absOffset) continue;
        _segments.push_back(_segment);
      }

      gex::MultiLineString _output;
      gex::join(_segments,_output,gex::algorithm::strategy::ThresholdWithReverse(0.001));
      return _output;
    }
  }

  using algorithm::offsetDetails;
}
