#include <gex/prim.hpp>
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/pattern.hpp>
#include <gex/algorithm/fill.hpp>

#include "create.hpp"
#include <gex/io/SVG.hpp>
#include <gex/algorithm/join.hpp>
#include <gex/algorithm/union.hpp>


template<typename GENERATOR, typename SHAPE>
void testPattern(gex::io::SVG& _svg, GENERATOR _gen, const SHAPE& _shape, const std::string& _name)
{
  using namespace gex;

  auto&& _pattern = fill(_shape,_gen);
  _svg.draw(_shape,"stroke:red;fill:none");
  _svg.draw(_pattern,"stroke:blue;fill:none");

  LineString _lineString;
  _svg.buffer().write("Pattern_"+_name+".svg");
  _svg.clear();
  
  join(_pattern,_lineString,algorithm::strategy::Simple());
  _svg.draw(_shape,"stroke:red;fill:none");
  _svg.draw(_lineString,"stroke:orange;fill:none");
  _svg.buffer().write("Pattern_"+_name+"_joined.svg");
  _svg.clear();
}


int main(int argc, char* argv[])
{
  using namespace gex;

  io::SVG _svg;
  auto&& _circleA = create::circle(Point2(-3,-3),5,false,48);
  auto&& _circleB = create::circleWith3Holes(Point2( 3, 3),5,48);


  MultiPolygon _shape;
  union_(_circleA,_circleB,_shape);
  _svg.buffer().fit(Bounds2(Point2(-10,-10),Point2(10,10)));
  _shape.update();

  typedef algorithm::pattern::Linear<Point2> Linear;
  typedef algorithm::pattern::Hexagon<Point2> Hexagon;
  typedef algorithm::pattern::Concentric<MultiPolygon,Point2> Concentric;

  Linear _linear(10,1.0);
  Hexagon _hexagon(10,1.0,0.1);
  Concentric _concentric(_shape,0.1);

  testPattern(_svg,_linear,_shape,"linear");
  testPattern(_svg,_hexagon,_shape,"hexagon");
  testPattern(_svg,_concentric,_shape,"concentric");

  return EXIT_SUCCESS;
}


