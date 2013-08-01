#include <gex/prim.hpp>
#include <gex/io.hpp>
#include <gex/algorithm/for_each.hpp>
#include "create.hpp"

gex::Scalar hexagonSideLength_ = 1.0;
static const gex::Scalar sin60_ = 0.5 * std::sqrt(3.0);

enum JointMode { JM_0, JM_60, JM_120, JM_180, JM_240, JM_300, JM_360 };


template<typename BOUNDS, typename POINT, typename LINESTRINGS>
void generateLine(
    const BOUNDS& _bounds, 
    const POINT& _point, 
    const gex::Scalar& _hexagonSideLength, LINESTRINGS& _ls,
    JointMode jointMode = JM_0)
{
  gex::LineString _line;
  _line.emplace_back(_point);
  bool _straight = true;

  while (_line.back().x() <= _bounds.max().x() &&
         _line.back().y() <= _bounds.max().y())
  {
    gex::Vec2 _delta;
    if (_straight)
    {
      _delta(_hexagonSideLength,0);
      _line.push_back(_line.back() + _delta);
    }
    else
    {
      _delta(_hexagonSideLength*0.5,_hexagonSideLength*sin60_);
      gex::Point2 _p(_line.back() + _delta);

      gex::LineString _joint;
      _joint.push_back(_p);
      _joint.push_back(_p + gex::Vec2(-_delta.x(),_delta.y()));
      _ls.push_back(_joint);
      _line.push_back(_p);
    }
    _straight = !_straight;
  }

  _ls.push_back(_line);
}

template<typename BOUNDS>
gex::MultiLineString hexagon(
    const BOUNDS& _bounds, 
    const gex::Scalar& _hexagonSideLength,
    JointMode jointMode = JM_0)
{
  gex::MultiLineString _hexagonPattern;

  for (gex::Scalar _y = _bounds.min().y() ; _y < _bounds.max().y(); _y += _hexagonSideLength*sin60_*2)
  {
    generateLine(_bounds,gex::Point2(_bounds.min().x(),_y),_hexagonSideLength,_hexagonPattern);
  }

  for (gex::Scalar _x = _bounds.min().x() + (1.5)*_hexagonSideLength; _x <= _bounds.max().x(); _x += 3*_hexagonSideLength)
  {
    generateLine(_bounds,gex::Point2(_x,_bounds.min().y() - _hexagonSideLength*sin60_),_hexagonSideLength,_hexagonPattern);
  }


  return _hexagonPattern;
}

template<typename BOUNDS, typename VERTICES, typename PRIMITIVE>
void warp(const BOUNDS& _bounds, const VERTICES& _vertices, PRIMITIVE& _lineStrings)
{
  auto&& _c = _bounds.center();
  gex::algorithm::for_each_point(_lineStrings,[&](gex::Point2& _point)
  {
    gex::Vertex2 _d(_bounds.size());
    for (auto& _v : _vertices)
    {
      gex::Vec2 _dist = _point - _v.v();
      if (_dist.length() < (_point - _d.v()).length()) _d(_v.v(),_v.n()); 
    }

    gex::Scalar _maxLength = 16*hexagonSideLength_*hexagonSideLength_;
/*
    if (_d.sqrLength() > _maxLength)
    {
      _d *= _maxLength / _d.sqrLength();
    }
*/
    _point -= _d.n();
  });
}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  _circle = create::circle(gex::Point2(0,0),7,true,48);
  _svg.buffer().fit(_bounds);
  _svg.draw(_circle,"stroke:red");

  std::vector<gex::Vertex2> _vertices;

  const int _numCells = 10;

  for (int x = 0; x < _numCells; ++x)
    for (int y = 0; y < _numCells; ++y)
    {
      gex::Point2 _p(x,y);
      _p[0] *= (1.0 / _numCells) * _bounds.size().x();
      _p[1] *= (1.0 / _numCells) * _bounds.size().y();
      _p += _bounds.min();
      
      gex::Vec2 _n((x-5) / 10.0 * 4 *  hexagonSideLength_,0);
      _vertices.emplace_back(_p,_n);
    }

  auto&& _hexagonPattern = hexagon(_bounds,hexagonSideLength_);
//  warp(_bounds,_vertices,_hexagonPattern);

  for (auto& _line : _hexagonPattern)
  {
    _svg.draw(_line,_line.size() == 2 ? "stroke:yellow;fill:none" : "stroke:green;fill:none");
  }
  _svg.buffer().write("hexagon.svg");

  return EXIT_SUCCESS;
}
