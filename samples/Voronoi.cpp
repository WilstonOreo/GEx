#include <gex.hpp>
#include "create.hpp"

#include <gex/index.hpp>
#include <gex/algorithm/voronoi.hpp>


#include <gex/index.hpp>
#include <gex/algorithm/boost.hpp>
#include <gex/algorithm/intersection.hpp>
#include <gex/io/svg/RandomColor.hpp>

template<typename PRIMITIVES>
void drawWithRandomColor(gex::io::SVG& _svg, const PRIMITIVES& _primitives)
{
  for (auto& _primitive : _primitives)
  {
    _svg.draw(_primitive,"stroke:"+gex::io::svg::randomColor()+";fill:none");
  }
}


template<typename VORONOI>
void drawVoronoi(gex::io::SVG& _svg, const VORONOI& _voronoi, const std::string& _color)
{
  gex::MultiLineString _in;
  _svg.draw(_voronoi,"stroke:"+_color+";fill:none");
  gex::algorithm::correct(_in);
}

template<typename POINTS>
void pointsTest(gex::io::SVG& _svg, const POINTS& _points)
{
  _svg.clear();
  auto&& _segments = gex::algorithm::voronoi(_points);
  drawVoronoi(_svg,_segments,"white");
  _svg.buffer().write("pointTest.svg");
}

template<typename RING>
void ringTest(gex::io::SVG& _svg, const RING& _ring)
{
  _svg.clear();
  _svg.draw(_ring,"stroke:red;fill:none");
  auto&& _segments = gex::algorithm::voronoi(_ring);
  drawVoronoi(_svg,_segments,"white");
  _svg.buffer().write("ringTest.svg");
}

template<typename POLYGON>
void polygonTest(gex::io::SVG& _svg, const POLYGON& _polygon)
{
  _svg.clear();
  _svg.draw(_polygon,"stroke:red;fill:none");
  auto&& _segments = gex::algorithm::voronoi(_polygon);
  drawVoronoi(_svg,_segments,"white");
  _svg.buffer().write("polygonTest.svg");
}

template<typename RING>
void ringSkeletonTest(gex::io::SVG& _svg, const RING& _ring)
{
  _svg.clear();
  auto&& _segments = gex::algorithm::skeleton(_ring);
  drawWithRandomColor(_svg,_segments);
  _svg.draw(_ring,"stroke:red;fill:none");
  
  std::cout << "ringSkeletonTest : " << _segments.size() << std::endl;
  _svg.buffer().write("ringSkeletonTest.svg");
}

template<typename POLYGON>
void polygonSkeletonTest(gex::io::SVG& _svg, const POLYGON& _polygon)
{
  _svg.clear();
  auto&& _segments = gex::algorithm::skeleton(_polygon);
  _svg.draw(_polygon,"stroke:red;fill:none");
  drawWithRandomColor(_svg,_segments);
  
  std::cout << "polygonSkeletonTest : " << _segments.size() << std::endl;
  _svg.buffer().write("polygonSkeletonTest.svg");
}

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto&& _circleWithHoles = create::circleWith3Holes(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  _circle = create::irregular(gex::Point2(0,0),4,true,32);
  /*
  for (auto& _p : _circle)
    {
      _p += gex::Point2(gex::rnd()*2 - 1,gex::rnd()*2 - 1);
    }*/

  _bounds = _circle.bounds();
  _circle.update();
  gex::algorithm::correct(_circle);
  _svg.draw(_circle,"stroke:red;fill:none");
  gex::MultiPoint2 _points;
  for (auto& _p : _circle)
    _points.push_back(_p);
  pointsTest(_svg,_points);

  ringTest(_svg,_circle);
  polygonTest(_svg,_circleWithHoles);

  /// Test skeleton
  ringSkeletonTest(_svg,_circle);
  polygonSkeletonTest(_svg,_circleWithHoles);

  return EXIT_SUCCESS;
}
