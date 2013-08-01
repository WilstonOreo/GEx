#include <gex.hpp>
#include <gex/algorithm/for_each.hpp>
#include <gex/polygon.hpp>
#include "create.hpp"

#include <boost/polygon/voronoi.hpp>

struct Segment
{
  gex::polygon::Point2 p0, p1;

  Segment( const gex::polygon::Point2& _p0,
           const gex::polygon::Point2& _p1) : p0(_p0), p1(_p1) {}

  Segment (int x1, int y1, int x2, int y2) : p0(x1, y1), p1(x2, y2) {}
};

template <>
struct boost::polygon::geometry_concept<Segment>
{
  typedef segment_concept type;
};

template <>
struct boost::polygon::point_traits<Segment>
{
  typedef int coordinate_type;
  typedef gex::polygon::Point2 point_type;

  static inline coordinate_type get(const Segment& segment, direction_1d dir)
  {
    return dir.to_int() ? segment.p1.x() : segment.p0.y();
  }
};

template<typename POINTSET, typename CELL>
gex::polygon::Point2 const& retrieve_point(const POINTSET& _points, const CELL& _cell)
{
  return _points[_cell.source_index()];
}

template<typename POINTSET, typename EDGE, typename BOUNDS, typename POINT>
void clip_infinite_edge(
  const POINTSET& _points,
  const EDGE& edge,
  const BOUNDS& _bounds,
  std::vector<POINT>& clipped_edge)
{
  typedef POINT point_type;
  typedef int coordinate_type; // @todo generic type here...

  auto& cell1 = *edge.cell();
  auto& cell2 = *edge.twin()->cell();
  point_type origin, direction;
  // Infinite edges could not be created by two segment sites.
  if (cell1.contains_point() && cell2.contains_point())
  {
    auto& p1 = retrieve_point(_points,cell1);
    auto& p2 = retrieve_point(_points,cell2);
    origin.x((p1.x() + p2.x()) * 0.5);
    origin.y((p1.y() + p2.y()) * 0.5);
    direction.x(p1.y() - p2.y());
    direction.y(p2.x() - p1.x());
  }
  else
  {
    return;
  }
  coordinate_type side = _bounds.size().y();
  coordinate_type koef =
    side / (std::max)(std::abs(direction.x()), std::abs(direction.y()));
  if (edge.vertex0() == NULL)
  {
    clipped_edge.emplace_back(origin.x() - direction.x() * koef,
                              origin.y() - direction.y() * koef);
  }
  else
  {
    clipped_edge.emplace_back(edge.vertex0()->x(), edge.vertex0()->y());
  }
  if (edge.vertex1() == NULL)
  {
    clipped_edge.emplace_back(origin.x() + direction.x() * koef,
                              origin.y() + direction.y() * koef);
  }
  else
  {
    clipped_edge.emplace_back(edge.vertex1()->x(), edge.vertex1()->y());
  }
}




template<typename PRIMITIVE, typename BOUNDS>
std::vector<gex::Segment> voronoi(const PRIMITIVE& _prim, const BOUNDS& _bounds)
{
  boost::polygon::voronoi_diagram<double> vd;

  std::vector<gex::polygon::Point2> _points;
  for (auto& _p : _prim)
  {
    _points.push_back(gex::polygon::adapt(_p,_bounds));
  }
  _points.pop_back();

  boost::polygon::construct_voronoi(_points.begin(), _points.end(),&vd);

  std::vector<gex::Segment> _segments;
  int result = 0;
  for (auto& _edge : vd.edges())
  {
 //   if (!_edge.is_primary())  continue;

    auto _v0 = _edge.vertex0();
    auto _v1 = _edge.vertex1();
    if (!_v0 || !_v1)
    {
      std::vector<gex::polygon::Point2> _clippedEdge;
      clip_infinite_edge(_points,_edge,_bounds,_clippedEdge);
      std::cout << "Clipped Edge: " << _clippedEdge.size() << std::endl;
      if (_clippedEdge.size() == 2)
      {
        _segments.emplace_back(
          gex::polygon::adapt(_clippedEdge[0],_bounds),
          gex::polygon::adapt(_clippedEdge[1],_bounds));
      }
      continue;
    }

    std::cout << _v0->x() << " " << _v0->y() << " ; " << _v1->x() << " " << _v1->y() << std::endl;
    gex::Point2 _p0 = gex::polygon::adapt(gex::polygon::Point2(
                                            _v0->x(),
                                            _v0->y()),_bounds);
    gex::Point2 _p1 = gex::polygon::adapt(gex::polygon::Point2(
                                            _v1->x(),
                                            _v1->y()),_bounds);

    std::cout << _p0 << " " << _p1 << std::endl;

    _segments.emplace_back(_p0,_p1);
  }
  return _segments;
}

#include <gex/index.hpp>
#include <gex/algorithm/boost.hpp>




#include <gex/algorithm/intersection.hpp>

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  _circle = create::irregular(gex::Point2(0,0),4,true,32);
/*
for (auto& _p : _circle)
  {
    _p += gex::Point2(gex::rnd()*2 - 1,gex::rnd()*2 - 1);
  }*/

  _bounds = _circle.bounds();


  _svg.draw(_circle,"stroke:red");

  /*  for (auto& _p : _polygonCircle)
      std::cout << _p.x() << " " << _p.y() << std::endl;
    for (auto& _p : _testCircle)
      std::cout << _p << std::endl;
  */
  auto&& _segments = voronoi(_circle,_bounds);
  //for (auto& _segment : _segments)
  //_svg.draw(_segment,"stroke:green");

  _circle.update();

  gex::MultiLineString _in, _out;

  for (auto& _segment : _segments)
  {
    gex::LineString _ls;
    _ls.push_back(_segment.p0());
    _ls.push_back(_segment.p1());
    _in.push_back(_ls);
  }

  gex::algorithm::correct(_circle);
  gex::algorithm::correct(_in);

  gex::algorithm::intersection(_in,_circle,_out);
  for (auto& _ls : _in)
  {
    _svg.draw(_ls,"stroke:white");
  }

  gex::algorithm::intersection(_in,_circle,_out);
for (auto& _ls : _out)
  {
    _svg.draw(_ls,"stroke:purple");
  }
  _svg.buffer().write("test.svg");


  return EXIT_SUCCESS;
}
