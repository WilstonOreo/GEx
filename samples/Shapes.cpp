#include "create.hpp"
#include <gex/io/SVG.hpp>
#include <gex/algorithm/pattern.hpp>
#include <gex/algorithm/fill.hpp>

int main(int argc, char* argv[])
{
  using namespace gex;

  io::SVG _svg;
  _svg.clear(1024,1024);

  Bounds2 _bounds(Point2(-10,-10),Point2(10,10));
  _svg.buffer().fit(_bounds);

  std::string _style = "stroke:white;fill:none;stroke-width:32px";

  size_t _id = 0;

  auto writeAndClear = [&_svg,&_id]()
  {
    std::stringstream _ss;
    _ss << std::setw(5) << std::setfill('0') << _id << ".svg"; 
    _svg.buffer().write(_ss.str());
    _svg.clear();
    ++_id;
  };

  ///// Rectangle 
  _svg.draw(Bounds2(Point2(-8,-8),Point2(8,8)),_style);
  writeAndClear();

  ///// Two rectangles
  _svg.draw(Bounds2(Point2(-8,-8),Point2(-1,8)),_style);
  _svg.draw(Bounds2(Point2(1,-8),Point2(8,8)),_style);
  writeAndClear();

  ///// Circle
  _svg.draw(create::circle(Point2(0.0,0.0),8,false,64),_style);
  writeAndClear();

  ///// Triangle
  _svg.draw(create::circle(Point2(0.0,0.0),8,false,3),_style);
  writeAndClear();

  ///// 5-gon
  _svg.draw(create::circle(Point2(0.0,0.0),8,false,5),_style);
  writeAndClear();
  
  ///// Star
  _svg.draw(create::star(Point2(0.0,0.0),8,3,false,10),_style);
  writeAndClear();

  ///// Circle with holes
  _svg.draw(create::circleWith3Holes(Point2(0.0,0.0),8,48),_style);
  writeAndClear();

  typedef algorithm::pattern::Linear<Point2> Linear;
  typedef algorithm::pattern::Hexagon<Point2> Hexagon;
  auto&& _shape = create::circle(Point2(0.0,0.0),9,false,80);
  
  ///// Horz Line Filling with circle
  auto&& _pattern = fill(_shape,Linear(0,3.0));
  _svg.draw(_pattern,_style);
  writeAndClear();
  

  ///// Vert Line Filling with circle
  _pattern = fill(_shape,Linear(90,3.0));
  _svg.draw(_pattern,_style);
  writeAndClear();

  ///// Wave Line
  _pattern = fill(_shape,Hexagon(0,2.0,4));
  _svg.draw(_pattern,_style);
  writeAndClear();
}
