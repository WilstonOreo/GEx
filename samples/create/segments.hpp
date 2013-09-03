#pragma once

namespace create
{

  std::vector<tomo::geometry::Segment> segments(size_t _nSegments = 100, float _segmentLength = 0.1f)
  {
    using tomo::geometry::Segment;
    using tomo::misc::rnd;

    typedef typename Segment::point_type point_type;
    std::vector<Segment> _segments;

    for (size_t i = 0; i < _nSegments; ++i)
    {
      point_type _p0(rnd(),rnd());
      point_type _p1(_p0.x() - (rnd() - 0.5f)*_segmentLength,
                     _p0.y() - (rnd() - 0.5f)*_segmentLength);
      _segments.push_back(Segment(_p0,_p1));
    }
    return _segments;
  }


}
