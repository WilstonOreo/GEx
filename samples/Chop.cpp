#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/convert.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/chop.hpp>
#include <gex/io.hpp>

using namespace gex;

void linestringTest(gex::io::SVG& _svg, gex::LineString& _lineString)
{
  typedef std::vector<gex::Scalar> Markers;
  Markers _markers({0.25,0.3333,0.5,0.75,0.9,0.95});
  _svg.clear();
  size_t _markerId = 0;
  _svg.draw(_lineString,"stroke:red;fill:none");

  auto&& _lineStrings = gex::algorithm::chop(_lineString,_markers);
  gex::algorithm::step(_lineString,_markers,[&](const Point2& _p)
  {
    _svg.buffer().text(_p,_markers[_markerId],"stroke:orange;fill:none");
    _svg.draw(_p,"stroke:orange;fill:none");
    _markerId++;
  },
  [&](const Point2& _p)
  {
    _svg.draw(_p,"stroke:blue;fill:none");
  });


  
  for (size_t i = 0; i < _lineStrings.size(); ++i)
  {
    auto& _lineString = _lineStrings[i];
    gex::Vec2 _offset(0.5,0.0);
    if (i % 2) 
    {
      _offset = -_offset;
    }
    
    for (auto& _p : _lineString) 
      _p += _offset;
    _svg.draw(_lineString,"stroke:orange;fill:none");
  }

  _svg.buffer().write("Chop_LineString.svg");
}

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;

  auto&& _circle = create::circle(gex::Point2(0,0),10,false,42);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  
  _circle = create::circle(gex::Point2(0,0),8,false,7);
  
  /*//  _circle = create::irregular(gex::Point2(),5,false,16);
    _circle = create::star(gex::Point2(),4,7,false,40);
  */

  typedef std::vector<double> Markers;

  Segment _segment(Point2(-5,-5),Point2(5,5));
  _svg.draw(_segment,"stroke:green");
  gex::algorithm::step(_segment,Markers( {0.0,0.25,0.5,0.75,0.9,1.0}),[&](const Point2& _p)
  {
    _svg.draw(_p,"stroke:red");
  });

  auto&& _segments = gex::algorithm::chop(_segment,Markers({0.25,0.5,0.75,0.9}));
  for (size_t i = 0; i < _segments.size(); ++i)
  {
    auto& _seg = _segments[i];
  
    gex::Vec2 _offset(0.5,0.0);
    if (i % 2) 
    {
      _offset = -_offset;
    }

    _seg.p0() += _offset;
    _seg.p1() += _offset;

    _svg.draw(_seg,"stroke:orange");
  }
  std::cout << _segments.size() << std::endl;

  Markers _markers({0.25,0.3333,0.5,0.75,0.9,0.95});
  _svg.draw(_circle,"stroke:green;fill:none");

  size_t _markerId = 0;

  gex::algorithm::step(_circle,_markers,[&](const Point2& _p)
  {
    _svg.buffer().text(_p,_markers[_markerId],"stroke:orange;fill:none");
    _svg.draw(_p,"stroke:orange;fill:none");
    _markerId++;
  },
  [&](const Point2& _p)
  {
    _svg.draw(_p,"stroke:blue;fill:none");
  });

  auto&& _lineStrings = gex::algorithm::chop(_circle,_markers);
  std::cout << _lineStrings.size() << std::endl;
  for (size_t i = 0; i < _lineStrings.size(); ++i)
  {
    auto& _lineString = _lineStrings[i];
    gex::Vec2 _offset(0.5,0.0);
    if (i % 2) 
    {
      _offset = -_offset;
    }
    
    for (auto& _p : _lineString) 
      _p += _offset;

    _svg.draw(_lineString,"stroke:orange;fill:none");
  }
  _svg.buffer().write("Chop.svg");


  ///// LineString test
  auto&& _lineString = gex::algorithm::convert<gex::LineString>(_circle);
  _lineString.back() += gex::Vec2(-1,1);
  linestringTest(_svg,_lineString);


  return EXIT_SUCCESS;
}

