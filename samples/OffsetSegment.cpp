#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/io.hpp>
#include <gex/algorithm/nonUniformOffset.hpp>

using namespace gex;


int main(int argc, char* argv[])
{
  using namespace gex;

  io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  //_circle = create::irregular(gex::Point2(),5,false,16);
  _svg.buffer().fit(_bounds);

  Segment _seg(Point2(-2,-4),Point2(4,-4));
  auto&& _ring = nonUniformOffset(_seg,std::make_pair(3,0.5));

  _svg.draw(_seg,"stroke:red;fill:none");
  _svg.draw(_ring,"stroke:lime;fill:none");

  _svg.buffer().write("NonUniformOffset.svg");

  return EXIT_SUCCESS;
}

