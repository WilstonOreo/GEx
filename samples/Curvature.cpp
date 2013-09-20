#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/io.hpp>

template<typename POINT>
typename POINT::scalar_type determinant(
  const POINT& _a,
  const POINT& _b,
  const POINT& _c)
{
  return (_b.x() * _c.y() + _a.x() * _b.y() + _a.y()*_c.x()) -
         (_a.y() * _b.x() + _b.y() * _c.x() + _a.x()*_c.y());
}

template<typename POINT>
typename POINT::scalar_type curvature(
  const POINT& _a,
  const POINT& _b,
  const POINT& _c)
{
}

template<typename PRIMITIVE, typename FUNCTOR>
void for_each_curve(const PRIMITIVE& _primitive, FUNCTOR f)
{
}




int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
//  _circle = create::irregular(gex::Point2(),5,false,16);
  _circle = create::star(gex::Point2(),4,7,false,40);


  //_circle.pop_back();
  boost::geometry::correct(_circle);

  _svg.draw(_circle,"stroke:green");

  std::multimap<gex::Scalar,Point2 const*> _candidates;

  for_each_curve(_circle,[&](const Point2& _p0, const Point2& _p1, const Point2& _p2)
  {
    auto&& _curvature = determinant(_p0,_p1,_p2);  
 //     ((_p2 - _p1).length() * (_p1 - _p0).length());
    
    if (_curvature > 0)
      _candidates.insert(std::make_pair(_curvature,&_p1));
  });

  for (auto& _candidate : _candidates)
  {
    _svg.draw(*_candidate.second,"stroke:yellow");
    _svg.buffer().text(*_candidate.second,_candidate.first,"font-size:15;fill:red");
  }



  _svg.buffer().write("Curvature.svg");

  return EXIT_SUCCESS;
}

