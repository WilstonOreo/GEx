#include <gex/io.hpp>
#include <gex/prim.hpp>
#include <gex/misc.hpp>
#include "create/fromTxt.hpp"
#include <gex/algorithm/util.hpp>

#include <gex/algorithm/shift.hpp>
#include <gex/algorithm/centroid.hpp>


#include <iostream>
#include <iomanip>

template<typename RING, typename INDEX>
void shiftTest(gex::io::SVG& _svg, const RING& _ring, const INDEX& _index)
{
  std::stringstream ss;
  for (float _d = 0.0; _d < 1.0; _d += 0.1)
  {
    _svg.clear();
    ss.str("");
    ss << std::setw(4) << std::setfill('0') << _index << "_" << _d << "_shift.svg";
    
    auto&& _shifted = gex::algorithm::shift(_ring,_d);
    
    gex::algorithm::for_each<gex::Point2>(_shifted,[&](const gex::Point2& _point)
    {
      _svg.draw(_point,"stroke:green;fill:none");
    });
    
    _svg.draw(_shifted.front(),"stroke:blue;fill:none");
  //  _svg.draw(_ring.front(),"stroke:yellow;fill:none");
    _svg.draw(_shifted,"stroke:red;fill:none");
    _svg.buffer().write(ss.str());
  }

  std::vector<gex::Point2> _points = { 
    gex::centroid(_ring), 
    gex::Point2(0,0) };

  for (auto& _point : _points)
  {
    _svg.clear();
    ss.str("");
    ss << std::setw(4) << std::setfill('0') << _index << "_" << _point.x();
    ss << "_" << _point.y() << "_point.svg";
    
    auto&& _shifted = gex::shift(_ring,_point);
    
    _svg.draw(_point,"stroke:red;fill:none");
    gex::algorithm::for_each<gex::Point2>(_shifted,[&](const gex::Point2& _p)
    {
      _svg.draw(_p,"stroke:green;fill:none");
    });
    
    _svg.draw(_shifted.front(),"stroke:blue;fill:none");
  //  _svg.draw(_ring.front(),"stroke:yellow;fill:none");
    _svg.draw(_shifted,"stroke:red;fill:none");
    _svg.buffer().write(ss.str());
  }

}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _rings = create::fromTxt("testcases.txt");

  if (_rings.empty()) return EXIT_FAILURE;

  gex::Bounds2 _bounds = _rings.front().bounds();

  for (auto& _ring : _rings)
  {
    _bounds.extend(_ring.bounds());
    std::cout << _ring.bounds() << " " << _bounds << std::endl;
  }

  _bounds.max().y() = _bounds.min().y() + _bounds.size().x();
  _svg.buffer().fit(_bounds);

  size_t _index = 0;
  for (auto& _ring : _rings)
  {
    _svg.clear();
    _svg.draw(_ring,"stroke:red;fill:none");

    gex::Point2 _point(4,2), _closestPoint;

    gex::algorithm::for_each<gex::Segment>(_ring,[&](const gex::Segment& _seg)
    {
      gex::algorithm::closestPoint(_seg,_point,_closestPoint);
      _svg.draw(_closestPoint,"stroke:blue;fill:none");
    });
    
    gex::algorithm::closestPoint(_ring,_point,_closestPoint);

    _svg.draw(_point,"stroke:green;fill:none");
    _svg.draw(_closestPoint,"stroke:yellow;fill:none");

    std::stringstream ss; ss << "ClosestPoint";
    ss << std::setw(4) << std::setfill('0') << _index << ".svg";
    _svg.buffer().write(ss.str());
    shiftTest(_svg,_ring,_index);

    _svg.clear();
    ++_index;
  }


  return EXIT_SUCCESS;
}
