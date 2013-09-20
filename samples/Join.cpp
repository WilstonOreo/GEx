#include <iostream>
#include <fstream>
#include <gex/io/SVG.hpp>
#include "create.hpp"

#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/envelope.hpp>
#include <gex/algorithm/join.hpp>
#include <gex/io/svg/RandomColor.hpp>

template<typename SEGMENTS>
void segmentsToLineStrings(gex::io::SVG& _svg, const SEGMENTS& _segments)
{
  namespace strategy = gex::algorithm::strategy;
  _svg.clear();
  gex::MultiLineString _lineStrings;
  gex::algorithm::join(_segments,_lineStrings,strategy::ThresholdWithReverse(0.01));

  _svg.draw(_lineStrings,"stroke:red;fill:none"); 
  std::cout << "Have " << _lineStrings.size() << " linestrings." << std::endl;
  _svg.buffer().write("Join_segments_linestrings.svg");
}

template<typename LINESTRINGS>
void linestringsToLinestringsTest(gex::io::SVG& _svg, const LINESTRINGS& _lineStrings)
{
  namespace strategy = gex::algorithm::strategy;
  _svg.clear();
  gex::MultiLineString _output;
  gex::algorithm::join(_lineStrings,_output,strategy::ThresholdWithReverse(0.01));

  _svg.draw(_output,"stroke:red;fill:none"); 
  std::cout << "Have " << _output.size() << " linestrings." << std::endl;
  _svg.buffer().write("Join_linestrings_linestrings.svg");
}

template<typename SEGMENTS>
void segmentsToRingsTest(gex::io::SVG& _svg, const SEGMENTS& _segments)
{
  namespace strategy = gex::algorithm::strategy;
  _svg.clear();
  gex::MultiRing _output;

  gex::MultiLineString _lineStrings;

  gex::algorithm::join(_segments,_output,strategy::DefectRings<gex::Point2>(_lineStrings,0.1));
 
  for (auto& _ring : _output)
  {
    std::cout << _ring.size() << std::endl;
    _svg.draw(_ring,"stroke:"+gex::io::svg::randomColor()+";fill:none"); 
  }
    
  std::cout << "Have " << _output.size() << " rings." << std::endl;
  _svg.buffer().write("Join_segments_rings.svg");
  
  _svg.clear();
  
  for (auto& _lineString : _lineStrings)
  {
    _svg.draw(_lineString,"stroke:"+gex::io::svg::randomColor()+";fill:none"); 
  }
  _svg.buffer().write("Join_segments_defect_rings.svg");
  
  std::cout << "Have " << _lineStrings.size() << " linestrings." << std::endl;
}

  template<typename SEGMENTS>
void segmentsToPolygonsTest(gex::io::SVG& _svg, const SEGMENTS& _segments)
{
  namespace strategy = gex::algorithm::strategy;
  _svg.clear();
  gex::MultiPolygon _output;
 // gex::algorithm::join(_segments,_output,strategy::Threshold(0.01));
  _svg.draw(_output,"stroke:red;fill:none"); 
  std::cout << "Have " << _output.size() << " polygons." << std::endl;
  _svg.buffer().write("Join_segments_polygons.svg");
}

/// Test in which segments exactly overlap
template<typename INPUT>
void exactTests(gex::io::SVG& _svg, const INPUT& _input)
{
  gex::MultiSegment _segments = gex::algorithm::convert<gex::MultiSegment>(_input);
  _segments.emplace_back(gex::Point2(6,6),gex::Point2(3,4));
  _segments.emplace_back(gex::Point2(1,3),gex::Point2(-1,-1));
  _segments.emplace_back(gex::Point2(1,3),gex::Point2(5,3));
 
  _svg.draw(_segments,"stroke:orange;fill:none");
  _svg.buffer().write("Join_segments.svg");

  segmentsToLineStrings(_svg,_segments);
  auto&& _lineStrings = gex::algorithm::convert<gex::MultiLineString>(_segments);
  linestringsToLinestringsTest(_svg,_lineStrings);

  auto&& _circle = create::irregular(gex::Point2(0,0),4,true,32);
  _segments = gex::algorithm::convert<gex::MultiSegment>(_circle);
  segmentsToRingsTest(_svg,_segments);
}

/// Test in which segments do not exactly overlap
void inexactTests(gex::io::SVG& _svg)
{
  using namespace gex;

  auto&& _circle = create::irregular(Point2(0,0),4,true,32);
  auto&& _segments = algorithm::convert<MultiSegment>(_circle);

  Scalar _eps = 0.001;
  auto _move = [&_eps](){ return (rnd()-0.5)*_eps; };

  for (auto& _segment : _segments)
  {
    _segment.back() += Vec2(_move(),_move());
    _segment.front() += Vec2(_move(),_move());
  }

  _svg.draw(_segments,"stroke:orange;fill:none");

  _svg.buffer().write("Join_inexact.svg");

  segmentsToRingsTest(_svg,_segments);

  auto&& _polygonSegments = gex::convert<MultiSegment>(create::circleWith3Holes(Point2(),4,32));
  for (auto& _segment : _polygonSegments)
  {
    _segment.back() += Vec2(_move(),_move());
    _segment.front() += Vec2(_move(),_move());
  }
  segmentsToRingsTest(_svg,_polygonSegments);
  segmentsToPolygonsTest(_svg,_polygonSegments);
}

#include <gex/io/read.hpp>

void meshSegmentsTest(const std::string& _filename, gex::io::SVG& _svg)
{
  using namespace gex;

  MultiSegment _segments;
  io::readWkt(_filename,_segments);

  std::cout << _filename << std::endl;

  Bounds2 _bounds;
  for (auto& _segment : _segments)
  {
    _bounds.extend(_segment.bounds());
  }
  _svg.buffer().fit(_bounds);
  _svg.draw(_segments,"stroke:red;fill:none");
  std::stringstream _ss;
  _ss << "Join_mesh_segments" << ".svg";
  _svg.buffer().write(_ss.str());

  segmentsToRingsTest(_svg,_segments);

}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  _circle = create::irregular(gex::Point2(0,0),4,true,32);
  _circle.back() += gex::Vec2(1,1);

  exactTests(_svg,_circle); 

  inexactTests(_svg);

  if (argc >= 2)
  {
    meshSegmentsTest(argv[1],_svg);
  }



/*  for (auto& _segment : _segments) 
  {
    _svg.draw(_segment,"stroke:orange;fill:none");
  }
*/


}


