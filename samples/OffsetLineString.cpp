#include <gex/io/SVG.hpp>
#include "create.hpp"

#include <gex/algorithm/offset.hpp>

template<typename SEGMENTS>
void segmentsTest(gex::io::SVG& _svg, const SEGMENTS& _segments)
{
  _svg.clear();
  for (auto& _segment : _segments)
  {
    auto&& _polygons = gex::offset(_segment,1);
    std::cout << _polygons.size() << std::endl;
    _svg.draw(_polygons,"stroke:red;fill:none");
  }
  _svg.buffer().write("Offset_segments.svg");
}

template<typename LINESTRING>
void lineStringTest(gex::io::SVG& _svg, const LINESTRING& _lineString)
{
  _svg.clear();
  auto&& _polygons = gex::offset(_lineString,1);
  std::cout << _polygons.size() << std::endl;
  _svg.draw(_polygons,"stroke:red;fill:none");
  _svg.buffer().write("Offset_linestring.svg");
}

template<typename RING, typename OFFSET>
void ringOffsetTest(gex::io::SVG& _svg, const RING& _ring, OFFSET _offset)
{
  _svg.clear();
  auto&& _polygons = gex::offset(_ring,_offset);
  _svg.draw(_polygons,"stroke:red;fill:none");

  std::stringstream ss; ss << "Offset_ring_" << _offset << ".svg";
  _svg.buffer().write(ss.str());
}

template<typename POLYGON, typename OFFSET>
void polygonOffsetTest(gex::io::SVG& _svg, const POLYGON& _polygon, OFFSET _offset)
{
  _svg.clear();
  auto&& _polygons = gex::offset(_polygon,_offset);
  _svg.draw(_polygons,"stroke:red;fill:none");
  std::stringstream ss; ss << "Offset_polygon_" << _offset << ".svg";
  _svg.buffer().write(ss.str());
}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  //_circle = create::irregular(gex::Point2(),5,false,16);
  _circle = create::star(gex::Point2(),3,7,false,8);
  auto&& _segments = gex::algorithm::convert<gex::MultiSegment>(_circle);
  segmentsTest(_svg,_segments);

  auto&& _lineString = gex::algorithm::convert<gex::LineString>(_circle);
  lineStringTest(_svg,_lineString);

  ringOffsetTest(_svg,_circle,1);
  ringOffsetTest(_svg,_circle,-1);

  auto&& _circleWithHoles = create::circleWith3Holes(gex::Point2(0,0),8);
  polygonOffsetTest(_svg,_circleWithHoles,1);
  polygonOffsetTest(_svg,_circleWithHoles,-1);

  return EXIT_SUCCESS;
}

