#include "create.hpp"
#include <gex/algorithm/reverse.hpp>

#include <gex/algorithm/for_each.hpp>


#include <gex/io.hpp>
#include <gex/algorithm/offset.hpp>

template<typename RING>
void testJunction(const RING& _ring, gex::io::SVG& _svg)
{
  typedef typename RING::point_type point_type;
  typedef gex::base::Curve<point_type> curve_type;

  _svg.draw(_ring,"stroke:red;fill:none");

  int i = 0;
  gex::for_each<curve_type>(_ring,[&](const curve_type& _curve)
  {
     if (i > 0) { i++; return; }
    RING _junction;
    gex::algorithm::functor::detail::generateJunction(
      _curve.p0(),
      _curve.p1(),
      _curve.p2(),
      0.2,0.001,_junction,_svg);
    _svg.draw(_junction,"stroke:blue;fill:none");
    i++;
  });

  gex::Polygon _polygon;
  gex::algorithm::functor::detail::generateOffsetPolygon(_ring,1.0,0.0001,_polygon,_svg);
  _svg.draw(_polygon,"stroke:green;fill:none");

  _svg.buffer().write("Offset_testJunction.svg");
  _svg.clear();
}

template<typename FILENAME> 
void testJunction(const FILENAME& _wkt)
{
  using namespace gex;
  io::SVG _svg;
  MultiPolygon _polygons;
  io::readWkt(_wkt,_polygons);

  std::vector<Scalar> _offsets = { 0.05, 0.1, 0.3, 0.5, 0.7, 1.0, 1.3, 2.0 };
  
  for_each<Ring>(_polygons,[&](Ring& _ring)
  {  
//    if (gex::algorithm::functor::detail::i < 10000) 
    {
  //    gex::algorithm::functor::detail::i += _offsets.size();
    //  return;
    }

    _ring.update();
    auto& _bounds = _ring.bounds();
    Ring _simplified;
    boost::geometry::simplify(_ring,_simplified,_bounds.size().norm() / 1000.0);

    boost::geometry::correct(_simplified);
    _simplified.update();
    
    for (auto& _offset : _offsets)
    {
      _svg.clear();
      _svg.buffer().fit(_ring.bounds());
      gex::Polygon _offsetPolygon;
      gex::algorithm::functor::detail::generateOffsetPolygon(_simplified,_offset,_bounds.size().norm() / 10000.0,_offsetPolygon);
    }
  });
}

void testJunctionAngles()
{
  using namespace gex;
  io::SVG _svg;

  
  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  
  const Scalar _radius = 8;
  const Scalar _offset = 1.5;
  
  for (float i = 0; i < 360; i += 10)
    for (float j = 0; j < 360; j += 10)
    {
      _svg.clear();
      Scalar _sin1 = _radius*sin(deg2rad(i)),
             _cos1 = _radius*cos(deg2rad(i));
      Scalar _sin2 = _radius*sin(deg2rad(j)),
             _cos2 = _radius*cos(deg2rad(j));

      std::cout << _sin1 << " " << _cos1 << " " << _sin2 << _cos2 << std::endl;

      Ring _r;
      LineString _l;
      _l.push_back(Point2(_cos1,_sin1));
      _l.push_back(Point2(0.0,0.0));
      _l.push_back(Point2(_cos2,_sin2));

      gex::algorithm::functor::detail::generateJunction(
        _l[0],
        _l[1],
        _l[2],
      _offset,0.001,_r);

      _svg.draw(_l,"stroke:green;fill:none");
      _svg.draw(_r,"stroke:blue;fill:none");

      std::stringstream _ss;
      _ss << "offset_test/" 
          << std::setw(5) << std::setfill('0') << i << "_" 
          << std::setw(5) << std::setfill('0') << j << "_" << _offset;
      _svg.buffer().write(_ss.str()+".svg");
    }
}


#include <gex/polygon.hpp>

template<typename FILENAME> 
void testBoostPolygonOffset(const FILENAME& _wkt)
{
  using namespace gex;
  io::SVG _svg;
  MultiPolygon _polygons;
  io::readWkt(_wkt,_polygons);

  std::vector<Scalar> _offsets = 
  { 
    -0.05, -0.1, -0.3, -0.5, -0.7, -1.0, -1.3, -2.0,
    0.05, 0.1, 0.3, 0.5, 0.7, 1.0, 1.3, 2.0 
  };
  
  size_t i = 0;
  for_each<Ring>(_polygons,[&](Ring& _ring)
  {
    _svg.clear();
    std::cout << i  << std::endl;
    _ring.update();
    auto& _bounds = _ring.bounds();
    Ring _simplified;

    Scalar _limit = _bounds.size().norm() / 10000.0;
    boost::geometry::simplify(_ring,_simplified,_limit);

    _svg.buffer().fit(_bounds);
    boost::geometry::correct(_simplified);
    _svg.draw(_simplified,"stroke:red;fill:none");
    _simplified.update();
    
    for (auto& _offset : _offsets)
    {
      auto&& _polygons = gex::polygon::adapt(gex::convert<gex::MultiPolygon>(_simplified),_bounds);
      _polygons.resize(_offset/_bounds.size().norm()*gex::polygon::size(),true,std::max(4,std::min(int(std::abs(_offset) / _limit),16)));
      _svg.draw(gex::polygon::adapt(_polygons,_bounds),"stroke:green;fill:none");
    }
    ++i;
    std::stringstream _ss;
    _ss << "offset_test/" 
        << std::setw(5) << std::setfill('0') << i;
    _svg.buffer().write(_ss.str()+".svg");
  });
}

template<typename FILENAME>
void testOffset(const FILENAME& _wkt)
{
  using namespace gex;
  io::SVG _svg;
  MultiPolygon _polygons;
  io::readWkt(_wkt,_polygons);

  std::vector<Scalar> _offsets = 
  { 
    -0.05, -0.1, -0.3, -0.5, -0.7, -1.0, -1.3, -2.0,
    0.05, 0.1, 0.3, 0.5, 0.7, 1.0, 1.3, 2.0 
  };

  size_t i = 0;
  for (auto& _polygon : _polygons)
  {
    std::cout << i << std::endl;
    _polygon.update();
    auto& _bounds = _polygon.bounds();
    
    _svg.clear();
    _svg.buffer().fit(_bounds);

    for (auto& _offset : _offsets)
    {
      auto&& _offsetPolygons = gex::offset(_polygon,_offset);
      _svg.draw(_polygon,"stroke:red;fill:none"); 
      _svg.draw(_offsetPolygons,"stroke:green;fill:none"); 
    }

    ++i;
    std::stringstream _ss;
    _ss << "offset_test/" 
        << std::setw(5) << std::setfill('0') << i;
    _svg.buffer().write(_ss.str()+".svg");
  }
}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
//  _circle = create::irregular(gex::Point2(0.0,0.0),8,false,16);
  _circle = create::star(gex::Point2(0.0,0.0),3,7,false,20);

  testOffset(argv[1]);

/*  testBoostPolygonOffset(argv[1]);

  testJunction(_circle,_svg);
  if (argc > 1)
  {
    testJunction(argv[1]);
  } else
  {
    testJunctionAngles();
  }
*/

  return EXIT_SUCCESS;
}
