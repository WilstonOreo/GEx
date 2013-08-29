#include <iostream>
#include <fstream>
#include <gex/io/SVG.hpp>
#include "create.hpp"
#include <gex/algorithm/connect.hpp>




int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  _circle = create::irregular(gex::Point2(0,0),4,true,32);
}

