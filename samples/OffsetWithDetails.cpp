#include <gex/algorithm/offset.hpp>
#include <gex/io/SVG.hpp>
#include <gex/io/read.hpp>
#include <gex/io/svg/RandomColor.hpp>

#include <gex/algorithm/perimeter.hpp>
#include <gex/algorithm/intersection.hpp>
//#include <gex/algorithm/offset_details.hpp>
#include <gex/algorithm/functor/Offset.hpp>
#include <gex/algorithm/medial_axis.hpp>
#include <gex/algorithm/closestPoint.hpp>

#include <gex/polygon.hpp>

namespace gex
{
  namespace algorithm
  {
    template<typename POLYGON, typename JUNCTIONS, typename OFFSET>
    void generateJunctions(
        const POLYGON& _polygon, 
        JUNCTIONS& _junctions,
        OFFSET _offset)
    {
      typedef typename POLYGON::point_type point_type;
      typedef typename POLYGON::ring_type ring_type;
      typedef base::Curve<point_type> curve_type;
      for_each<curve_type>(_polygon,[&](const curve_type& _curve)
      {
        ring_type _junction;
            gex::algorithm::functor::detail::generateJunction(
              _curve.p0(),
              _curve.p1(),
              _curve.p2(),
              std::abs(_offset),std::abs(_offset * 10e-5),_junction);
        _junctions.push_back(_junction);
      });
    }

    template<typename POLYGON, typename OFFSET> 
    gex::MultiPolygon junctionPolygon(
        const POLYGON& _polygon, 
        OFFSET _offset)
    {
      typedef typename POLYGON::ring_type ring_type;
      gex::MultiRing _minJunctions;
      gex::MultiRing _offsetJunctions;
      generateJunctions(_polygon,_offsetJunctions,std::abs(_offset));

      auto& _pbounds = _polygon.bounds();
      auto&& _center = _pbounds.center();
      auto&& _max = (_pbounds.size()(X) + _pbounds.size()(Y)) / 2.0;
      Vec2 _m(_max,_max);
      Bounds2 _bounds(_center - _m,_center + _m);
      
      using namespace boost::polygon::operators;
      gex::polygon::MultiPolygon _intersections;

      for (auto& _junction : _offsetJunctions)
      {
        _intersections += gex::polygon::adapt(_junction,_bounds);
      }

      return gex::polygon::adapt(_intersections,_bounds);
    }
  }
}

#include "create/circle.hpp"

template<typename SEGMENTS, typename POLYGON, typename SVG>
void paintCirclesFromAxis(const SEGMENTS& _segments, const POLYGON& _polygon, SVG& _svg)
{

  for (auto& _segment : _segments)
  {
    auto&& _p = gex::closestPoint(_polygon,_segment.front());
  
    gex::Scalar _d = gex::distance(_p,_segment.front());

    _svg.draw(_segment.front(),"stroke:yellow;fill:none");
    _svg.draw(_p,"stroke:orange;fill:none");
    auto&& _circle = create::circle(_segment.front(),_d);
    _svg.draw(_circle,"stroke:red;fill:none");
  }
}


/*
template<typename RTREE, typename SEGMENT>
bool innerSegment(const RTREE& _rtree, const SEGMENT& _segment)
{
  _rtree::query(gex::index::nearest(_segment.front()),std::back_inserter(_candidates));
  if (_candidates.empty()) return false;

  _rtree::query(gex::index::nearest(_segment.back()),std::back_inserter(_candidates));
  if (_candidates.empty()) return false;

  return true;
}
template<typename RTREE, typename SEGMENT, typename FUNCTOR>
void traversal(const RTREE& _rtree, const SEGMENT& _segment, FUNCTOR f)
{
  SEGMENT const* _s = &_segment;
  while (1) 
  {
    f(*s);
    std::vector<> _candidates;
    _rtree.query(gex::index::intersects(_segment.bounds()))
    if (_candidates.empty()) return;
    
    _s = _candidates.front();
  }
}


template<typename SEGMENTS, typename POLYGON, typename SVG>
void segments()
{
  gex::RTree2<gex::Segment const*> _rtree;
  for (auto& _segment : _segments)
  {
    _rtree.insert({_segment.bounds(),&_segment});
  }
}*/




int main(int argc, char* argv[])
{
  using namespace gex;
  std::string _wkt(argv[1]);

  MultiPolygon _polygons;
  MultiPolygon _offsetPolygons;
  io::readWkt(_wkt,_polygons);

  float _extrusionWidth = 0.1;
  std::vector<Scalar> _offsets = { -1.5, -0.5 };
  
  size_t _number = 0;
  
  for (auto& _polygon : _polygons)
  {
    io::SVG _svg;
    _polygon.update();
    auto& _bounds = _polygon.bounds();
    auto&& _center = _bounds.center();
    auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
    Vec2 _m(_max,_max);
    _svg.buffer().fit(Bounds2(_center - _m,_center + _m));

    for (auto& _offset : _offsets)
    {
      _svg.clear();
      _svg.draw(_polygon,"stroke:red;fill:none");
      std::stringstream _ss;
      _ss << "offset_with_details/" << std::setw(5) << std::setfill('0') << _number << "_" << _offset;
      _offsetPolygons.clear();
      gex::offset(_polygon,_offset,_offsetPolygons);

      gex::MultiPolygon _perimeter, _outer;
      for (auto& _p : _offsetPolygons) 
        _p.update();
      gex::offset(_offsetPolygons,_extrusionWidth * 0.5,_perimeter);
      gex::offset(_polygon,_offset + _extrusionWidth,_outer);
    
      auto&& _medial_axis = gex::medial_axis(_polygon,gex::strategy::medial_axis::SegmentsOnly());
      auto&& _scaleAxis = gex::medial_axis(_polygon,gex::strategy::medial_axis::ScaleAxis(std::abs(_offset)));

      for (auto& _segment : _medial_axis)
      {
        _svg.draw(_segment,"stroke:gray;fill:none");
      }
   
      for (auto& _segment : _scaleAxis)
      {
        _svg.draw(_segment,"stroke:"+gex::io::svg::randomColor()+";fill:none");
      }

/*
      gex::MultiPolygon _junctionPolygon = gex::algorithm::junctionPolygon(_offsetPolygons,_extrusionWidth*0.5);
      _svg.draw(_junctionPolygon,"stroke:yellow;fill:none");

      auto&& _segments = gex::offsetDetails(_polygon,_offsetPolygons,_offset,_extrusionWidth);
      gex::MultiRing _junctions;
      gex::algorithm::generateJunctions(_polygon,_junctions,std::abs(_offset));
      _svg.draw(_junctions,"stroke:orange;fill:none");
*/
      
/*
      paintCirclesFromAxis(_segments,_polygon,_svg);
      gex::MultiPolygon _intersections;
      gex::algorithm::junctionIntersections(_polygon,_offset,_extrusionWidth,_intersections);

      for (auto& _p : _intersections)
      {
         _p.update();
         auto&& _medial_axis = gex::medial_axis(_polygon);
        _svg.draw(_medial_axis,"stroke:orange;fill:none");
      }

      _svg.draw(_intersections,"stroke:white;fill:none");
*/
      std::cout << "Perimeter = " << gex::perimeter(_offsetPolygons) << std::endl;
/*
      _svg.draw(_offsetPolygons,"stroke:green;fill:none");
      _svg.draw(_perimeter,"stroke:orange;fill:none");*/
      _svg.buffer().write(_ss.str()+".svg");
      _number++;
    }
  }
}
