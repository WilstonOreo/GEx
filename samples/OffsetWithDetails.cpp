#include <gex/algorithm/offset.hpp>
#include <gex/io/SVG.hpp>
#include <gex/io/read.hpp>
#include <gex/io/svg/RandomColor.hpp>

//#include <gex/algorithm/offset_details.hpp>
#include <gex/algorithm/medial_axis.hpp>

#include <gex/polygon.hpp>

#include "create/circle.hpp"


template<typename SEGMENTS, typename POLYGON, typename SVG>
void paintCirclesFromAxis(const SEGMENTS& _segments, const POLYGON& _polygon, SVG& _svg)
{
  using namespace gex;

for (auto& _segment : _segments)
  {
    auto&& _p0 = closestPoint(_polygon,_segment.p0());
    auto&& _p1 = closestPoint(_polygon,_segment.p1());
    Scalar _d0 = distance(_p0,_segment.p0());
    Scalar _d1 = distance(_p1,_segment.p1());

    auto&& _ring = nonUniformOffset(_segment,std::make_pair(_d0,_d1));
    _svg.draw(_ring,"stroke:yellow;fill:none");
  }
}

template<typename POLYGON>
void fitSVG(const POLYGON& _polygon, gex::io::SVG& _svg)
{
  using namespace gex;
  auto& _bounds = _polygon.bounds();
  auto&& _center = _bounds.center();
  auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
  Vec2 _m(_max,_max);
  _svg.buffer().fit(Bounds2(_center - _m,_center + _m));
}

  template<typename POLYGON, typename SVG>
void drawPolygon(const POLYGON& _polygon, SVG& _svg)
{
  _svg.buffer().resize(512,512);
  fitSVG(_polygon,_svg);
  _svg.buffer().backgroundColor() = gex::io::svg::Color("white");
  _svg.draw(_polygon,"stroke:black;fill:none");
}

template<typename POLYGON, typename SVG>
void drawPolygonSimplifiedMedialAxis(const POLYGON& _polygon, float _factor, SVG& _svg)
{
  drawPolygon(_polygon,_svg);
  auto&& _medial_axis = gex::medial_axis(_polygon,gex::strategy::medial_axis::SegmentsOnly());
  _svg.draw(_medial_axis,"stroke:dimgray;fill:none");

  gex::Polygon _simplified;
  gex::algorithm::simplify(_polygon,_simplified,_factor*0.5);
  _simplified.update();
  _svg.draw(_simplified,"stroke:orange;fill:none");
}

template<typename BRANCHES, typename SVG>
void drawBranches(const BRANCHES& _branches, const std::string& _color, SVG& _svg)
{
  for (auto& _branch : _branches)
  {
    auto&& _segments = _branch->getSegments();

    for (auto _segment : _segments)
    {
      _svg.draw(*_segment,"stroke:"+_color+";fill:none");
    }
  }
}

template<typename POLYGON>
void svgMedialAxis(const POLYGON& _polygon, float _factor, const std::string& _prefix)
{
  using namespace gex;
  io::SVG _svg;
  drawPolygonSimplifiedMedialAxis(_polygon,_factor,_svg);
  _svg.buffer().write(_prefix+"simplified.svg");
}

template<typename POLYGON>
void svgPrimaryBranches(const POLYGON& _polygon, float _factor, const std::string& _prefix)
{
  using namespace gex;
  typedef algorithm::functor::Branch<gex::Segment> branch_type;
  typedef base::Curve<Point2> curve_type;
  typedef std::unique_ptr<curve_type> curve_ptr_type;

  io::SVG _svg;
  drawPolygonSimplifiedMedialAxis(_polygon,_factor,_svg);
  auto&& _medial_axis = gex::medial_axis(_polygon,gex::strategy::medial_axis::SegmentsOnly());
  auto&& _branches = gex::algorithm::functor::get_branches(_medial_axis);
  /*
    for (auto& _branch : _branches)
    {
      _svg.draw(_branch.startPoint(),"stroke:lime;fill:none");
    }
    */

  std::vector<branch_type const*> _primary, _secondary;
  std::vector<curve_ptr_type> _curves;
  gex::Polygon _simplified;
  gex::algorithm::simplify(_polygon,_simplified,_factor*0.5);
  _simplified.update();
  gex::algorithm::functor::get_curves(_simplified,_curves);
  gex::algorithm::functor::get_primary_and_secondary_branches(_branches,_curves,_primary,_secondary);

  drawBranches(_secondary,"red",_svg);
  drawBranches(_primary,"blue",_svg);
  _svg.buffer().write(_prefix+"branches.svg");
}

template<typename POLYGON>
void svgSecondaryPruned(const POLYGON& _polygon, float _factor, const std::string& _prefix)
{
  using namespace gex;
  typedef algorithm::functor::Branch<gex::Segment> branch_type;
  typedef base::Curve<Point2> curve_type;
  typedef std::unique_ptr<curve_type> curve_ptr_type;
  auto&& _medial_axis = gex::medial_axis(_polygon,gex::strategy::medial_axis::SegmentsOnly());

  gex::Polygon _simplified;
  gex::algorithm::simplify(_polygon,_simplified,_factor*0.5);
  _simplified.update();
  gex::MultiSegment _pruned;

  int _iteration = 0;
  while (_medial_axis.size() != _pruned.size())
  {
    io::SVG _svg;
    drawPolygon(_polygon,_svg);
    std::vector<curve_ptr_type> _curves;
    gex::algorithm::functor::get_curves(_simplified,_curves);
    auto&& _branches = gex::algorithm::functor::get_branches(_medial_axis);

    std::vector<branch_type const*> _primary, _secondary;
    gex::algorithm::functor::get_primary_and_secondary_branches(_branches,_curves,_primary,_secondary);

    _pruned = gex::algorithm::functor::pruned(_medial_axis,_secondary);
    _svg.draw(_pruned,"stroke:red;fill:none");
    std::swap(_pruned,_medial_axis);
    std::stringstream _ss;
    _ss << _prefix << "pruned" << std::setw(3) << std::setfill('0') << _iteration << ".svg";
    _svg.buffer().write(_ss.str());
    _iteration++;
  }
}

template<typename POLYGON>
void svgPrimaryPruned(const POLYGON& _polygon, float _factor, const std::string& _prefix)
{
  using namespace gex;
  typedef algorithm::functor::Branch<gex::Segment> branch_type;
  typedef base::Curve<Point2> curve_type;
  typedef std::unique_ptr<curve_type> curve_ptr_type;
  auto&& _medial_axis = gex::medial_axis(_polygon,gex::strategy::medial_axis::SegmentsOnly());

  gex::Polygon _simplified;
  gex::algorithm::simplify(_polygon,_simplified,_factor*0.5);
  _simplified.update();
  io::SVG _svg;
  drawPolygonSimplifiedMedialAxis(_polygon,_factor,_svg);
  gex::MultiSegment _pruned = gex::algorithm::functor::prune_secondary(_simplified,_medial_axis);
  _svg.draw(_pruned,"stroke:blue;fill:none");

  auto&& _branches = gex::algorithm::functor::get_branches(_pruned);
  std::vector<branch_type const*> _branchPointers;
  for (auto& _branch : _branches) 
    _branchPointers.push_back(&_branch);

  gex::MultiLineString _ls, _left;
  auto&& _withoutBranches = gex::algorithm::functor::pruned(_pruned,_branchPointers);

  for (auto& _branch : _branches)
  {
    if (!_branch.freeEnd())
      _ls.push_back(_branch.asLineString());
    else
      gex::for_each<gex::Segment>(_branch.asLineString(),[&](const gex::Segment& _segment)
      {
        _withoutBranches.push_back(_segment);
      });
  }

  auto&& _offset = gex::offset(_polygon,-_factor);
  boost::geometry::correct(_offset);
  boost::geometry::correct(_ls);
  boost::geometry::intersection(_offset,_ls,_left);

  gex::for_each<gex::Segment>(_left,[&](const gex::Segment& _segment)
  {
    _withoutBranches.push_back(_segment);
  });
  _svg.draw(_withoutBranches,"stroke:red;fill:none");


  _svg.buffer().write(_prefix+"primary_pruned.svg");
}

template<typename POLYGON>
void svgFinal(const POLYGON& _polygon, float _factor, const std::string& _prefix)
{
  using namespace gex;
  io::SVG _svg;
  drawPolygon(_polygon,_svg);

  auto&& _medial_axis = medial_axis(_polygon,strategy::medial_axis::Pruning(_factor));
  _svg.draw(_medial_axis,"stroke:red;fill:none");

  _svg.buffer().write(_prefix+"final.svg");
}

template<typename POLYGON>
void svgPerimeter(const POLYGON& _polygon, float _factor, float _width, int _number, const std::string& _prefix)
{
  using namespace gex;

  io::SVG _svg;
  drawPolygon(_polygon,_svg);
  //_svg.draw(_medial_axis,"stroke:red;fill:none");
  gex::MultiPolygon _offsetPolygons, _lastPolygons;
  for (int i = 0; i < _number; i++)
  {
    float _offset = -_width*(i+0.5);
    offset(_polygon,strategy::offset::Default(_offset),_offsetPolygons);
    _lastPolygons = offset(_polygon,strategy::offset::Default(-_width*(i-0.5)));
    auto&& _polygons = offset(_polygon,strategy::offset::Default(-_width*(i)));
    for (auto& _p : _offsetPolygons)
      _p.update();
    for (auto& _p : _polygons)
      _p.update();
    _svg.draw(_offsetPolygons,"stroke:orange;fill:none");

    for (auto& _p : _lastPolygons)
      _p.update();

    gex::MultiLineString _remaining;
    auto&& _medial_axis = convert<MultiLineString>(medial_axis(_polygons,strategy::medial_axis::Pruning(_width)));
    //_svg.draw(_medial_axis,"stroke:green;fill:none");
    boost::geometry::correct(_medial_axis);
    boost::geometry::intersection(_medial_axis,_polygons,_remaining);
    _medial_axis.clear();
    join(_remaining,_medial_axis,strategy::join::ThresholdWithReverse(0.001));
    boost::geometry::correct(_medial_axis);
    boost::geometry::correct(_offsetPolygons);
    _remaining.clear();
    boost::geometry::difference(_medial_axis,_offsetPolygons,_remaining);
    boost::geometry::correct(_remaining);
    
    _svg.draw(_remaining,"stroke:green;fill:none");

    //_medial_axis = _remaining;
    //_remaining.clear();


    if (_offsetPolygons.empty()) break;
  }

  _svg.buffer().write(_prefix+"perimeter.svg");
}

template<typename POLYGON>
void svgThickening(const POLYGON& _polygon, float _factor, float _width, int _number, const std::string& _prefix)
{
  using namespace gex;

  io::SVG _svg;
  drawPolygon(_polygon,_svg);
  float _offset = -_width*0.5;

  auto&& _offsetPolygons = offset(_polygon,strategy::offset::Default(_offset));
  for (auto& _p : _offsetPolygons)
      _p.update();
  _svg.draw(_offsetPolygons,"stroke:orange;fill:none");

  gex::MultiLineString _remaining;
  auto&& _medial_axis = convert<MultiLineString>(medial_axis(_polygon,strategy::medial_axis::Pruning(_factor)));
    //_svg.draw(_medial_axis,"stroke:green;fill:none");
  boost::geometry::correct(_medial_axis);
  boost::geometry::difference(_medial_axis,_offsetPolygons,_remaining);
  
  auto _bounds = _polygon.bounds();

  MultiLineString _connected;
  join(_remaining,_connected,strategy::join::ThresholdWithReverse(0.001));

  MultiLineString _cleaned;
  for (auto& _s : _connected)
  {
    if (gex::sqrDistance(_s.front(),_s.back()) < 0.00001) continue;
    _cleaned.push_back(_s);
  }
  
  MultiLineString _connectedAll;
  join(_medial_axis,_connectedAll,strategy::join::ThresholdWithReverse(0.001));

  MultiLineString _cleanedAll;
  for (auto& _s : _connectedAll)
  {
    if (gex::sqrDistance(_s.front(),_s.back()) < 0.00001) continue;
    _cleanedAll.push_back(_s);
  }

  using namespace boost::polygon::operators;
  auto&& _thickened = polygon::adapt(offset(_polygon,strategy::offset::Default(_width*0.5)),_bounds);
  for (auto& _s : _cleaned)
  {
    for_each<Segment>(_s,[&](const Segment& _segment) 
    {
      auto&& _offsetPolygon = offset(_segment,strategy::offset::NonUniform<Segment>(std::make_pair(_width,_width)));
      _thickened += polygon::adapt(_offsetPolygon,_bounds);
    });
  }

  _svg.draw(polygon::adapt(_thickened,_bounds),"stroke:green;fill:none");
  
  _svg.draw(_cleaned,"stroke:red;fill:none");
  _svg.buffer().write(_prefix+"thickening.svg");
}



int main(int argc, char* argv[])
{
  using namespace gex;
  std::string _wkt(argv[1]);

  MultiPolygon _polygons;
  MultiPolygon _offsetPolygons;
  io::readWkt(_wkt,_polygons);

  size_t _number = 0;

  for (auto& _polygon : _polygons)
  {
    _polygon.update();
    if (_polygon.boundary().size() < 10) continue;
    std::stringstream _ss;
    _ss << "offset_with_details/" << std::setw(5) << std::setfill('0') << _number << "_";

    auto&& _prefix = _ss.str();
    float _factor = 0.5;
    std::cout << "Perimeter = " << gex::perimeter(_offsetPolygons) << std::endl;
/*
    svgMedialAxis(_polygon,_factor,_prefix);
    svgPrimaryBranches(_polygon,_factor,_prefix);
    svgSecondaryPruned(_polygon,_factor,_prefix);
    svgPrimaryPruned(_polygon,_factor,_prefix);
    svgFinal(_polygon,_factor,_prefix);
*/
    svgPerimeter(_polygon,_factor,_factor,5,_prefix);
    svgThickening(_polygon,_factor,_factor,5,_prefix);

    _number++;
  }
}
