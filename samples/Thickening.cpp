#include <gex/io/SVG.hpp>
#include <gex/algorithm/thicken.hpp>

#include "create.hpp"

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;
  auto&& _circle = create::circle(Point2(),10,false,42);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);

  auto&& _star = create::star(Point2(),7,3,false,12);

  _svg.draw(_star,"stroke:orange;fill:none");

  auto&& _skeleton = gex::skeleton(_star);
  _svg.draw(_skeleton,"stroke:white;fill:none");
  
//  auto&& _voronoi = gex::voronoi(_star);
//  _svg.draw(_voronoi,"stroke:yellow;fill:none");

  gex::Scalar _offset = 3.0;

  auto&& _inner = gex::offset(_star,-_offset/4.0);
  _svg.draw(_inner,"stroke:blue;fill:none");

  auto&& _thickened = gex::thicken(_star,_offset);
  _svg.draw(_thickened,"stroke:green;fill:none");
  _svg.buffer().write("Thicken.svg");

  return EXIT_SUCCESS;
}

