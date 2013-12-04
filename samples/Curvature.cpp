#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/extremePoints.hpp>

template<typename POLYGON>
void fitSVG(const POLYGON& _polygon, gex::io::SVG& _svg)
{
  using namespace gex;
  auto& _bounds = _polygon.bounds();
  auto&& _center = _bounds.center();
  auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
  Vec2 _m(_max,_max);
  _svg.buffer().fit(Bounds2(_center - _m,_center + _m));
}



int main(int argc, char* argv[])
{
  using namespace gex;
  std::string _wkt(argv[1]);
  
  MultiPolygon _polygons;
  io::readWkt(_wkt,_polygons);
  
  size_t _number = 0;
  for (auto& _polygon : _polygons)
  {
    io::SVG _svg;
    _svg.buffer().backgroundColor() = gex::io::svg::Color("black");
    _polygon.update(); 
    fitSVG(_polygon,_svg);

    _svg.draw(_polygon,"stroke:white;fill:none");

    std::stringstream _ss;
    _ss << "curvature/Curvature" << std::setw(3) << std::setfill('0') << _number << ".svg";

    std::vector<Point2> _points;
    algorithm::extremePoints(_polygon.boundary(),_points,_svg);
    _svg.buffer().write(_ss.str());
    ++_number;
  }

  return EXIT_SUCCESS;
}

