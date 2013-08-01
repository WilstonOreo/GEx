#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/io.hpp>


template<typename POINT, typename RING>
void generateJunction( const POINT& _p0,
                       const POINT& _p1,
                       const POINT& _p2,
                       typename POINT::scalar_type _offset,
                       typename POINT::scalar_type _limit,
                       RING& _ring, gex::io::SVG& _svg)
{
  typedef POINT point_type;
  typedef typename POINT::scalar_type scalar_type;
  typedef typename RING::vec_type vec_type;

  using namespace gex::algorithm::functor;

  _ring.clear();
  if ((_p0 - _p1).length() < _limit)
    return;

  auto _n0 = detail::getNormal(_p0,_p1).normalized(),
       _n1 = detail::getNormal(_p1,_p2).normalized();

  _ring.clear();
  _ring.push_back(_p0 + _offset*_n0);
  scalar_type _a0 = std::atan2(_n1.y(),_n1.x()),
              _a1 = std::atan2(_n0.y(),_n0.x());

  if (_offset > 0 && _a1 < _a0) _a1 += M_PI * 2.0;
  else if (_offset < 0 && _a1 > _a0) _a1 -= M_PI * 2.0;

  if (_offset < 0)
  {
    std::swap(_a0,_a1);
  }

  if (_a1 - _a0 < M_PI)
  {
    if (_offset < 0) std::swap(_a0,_a1);
    detail::buildArc(_p1,_a1,_a0,_offset,_limit,_ring);
  }
  else
  {
    gex::Segment _seg0(_p0 + _offset * _n0,
                       _p1 + _offset * _n0),
                 _seg1(_p1 + _offset * _n1,
                       _p2 + _offset * _n1);
    _svg.draw(_seg0,"stroke:orange;fill:none");
    _svg.draw(_seg1,"stroke:yellow;fill:none");

    gex::Point2 _iPoint;
    if (detail::lineSegmentIntersection(_seg0,_seg1,_iPoint))
    {
      //_svg.draw(_seg0,"stroke:orange;fill:none");
      _svg.draw(_iPoint,"stroke:green;fill:none");
      _ring.push_back(_iPoint);
    }
    else
    {
      _ring.clear();
      _ring.push_back(_seg0.p0());
      _ring.push_back(_seg0.p1());
      _ring.push_back(_seg1.p0());
      _ring.push_back(_seg1.p1());
      _ring.update();
      return;
    }
  }
  _ring.push_back(_p2 + _offset * _n1);
  _ring.push_back(_p2);
  _ring.push_back(_p1);
  _ring.push_back(_p0);
  _ring.update();
}

template<typename RING, typename OFFSET, typename LIMIT>
void rawOffsetRing(    const RING& _in,
                       const OFFSET& _offset,
                       const LIMIT& _limit,
                       RING& _out, gex::io::SVG& _svg)
{
  typedef typename RING::point_type point_type;
  typedef typename RING::scalar_type scalar_type;
  typedef typename RING::vec_type vec_type;

  using namespace gex::algorithm::functor;
  _out.clear();
  auto _it = _in.begin();

  while (1)
  {
    if (_it == _in.end()-1) break;

    auto _p0 = *_it;
    point_type _p1, _p2;
    if (_it == _in.end()-2)
    {
      _p1 = _in.front();
      _p2 = _in[1];
    }
    else if (_it == _in.end()-3)
    {
      _p1 = *(_in.end()-2);
      _p2 = _in.front();
    }
    else
    {
      _p1 = *(_it+1);
      _p2 = *(_it+2);
    }
    ++_it;

    if ((_p0 - _p1).length() < _limit)
      return;

    auto _n0 = detail::getNormal(_p0,_p1).normalized(),
         _n1 = detail::getNormal(_p1,_p2).normalized();

//   _out.push_back(_p0 + _offset*_n0);
    scalar_type _a0 = std::atan2(_n1.y(),_n1.x()),
                _a1 = std::atan2(_n0.y(),_n0.x());

    if (_offset > 0 && _a1 < _a0) _a1 += M_PI * 2.0;
    else if (_offset < 0 && _a1 > _a0) _a1 -= M_PI * 2.0;

    if (_offset < 0)
    {
      std::swap(_a0,_a1);
    }

    if (_a1 - _a0 < M_PI)
    {
      if (_offset < 0) std::swap(_a0,_a1);
      detail::buildArc(_p1,_a1,_a0,_offset,_limit,_out);
    }
    else
    {
      gex::Segment _seg0(_p0 + _offset * _n0,
                         _p1 + _offset * _n0),
                             _seg1(_p1 + _offset * _n1,
                                   _p2 + _offset * _n1);
      _svg.draw(_seg0,"stroke:orange;fill:none");
      _svg.draw(_seg1,"stroke:yellow;fill:none");

      gex::Point2 _iPoint;
      if (detail::lineSegmentIntersection(_seg0,_seg1,_iPoint))
      {
        //_svg.draw(_seg0,"stroke:orange;fill:none");
        _svg.draw(_iPoint,"stroke:green;fill:none");
        _out.push_back(_iPoint);
      }
    }
    // _out.push_back(_p2 + _offset * _n1);
  }

  _out.update();
}






template<typename RING, typename OFFSET, typename OUTPUT>
void offsetRing(const RING& _ring,
                const OFFSET& _offset,
                OUTPUT& _output,
                gex::io::SVG& _svg,
                double _eps = 0.0001)
{
  typedef typename RING::scalar_type scalar_type;
  typedef typename RING::point_type point_type;
  typedef typename RING::vec_type vec_type;

  auto _limit = _ring.bounds().size().length() * _eps;

  RING _simpleRing;
  gex::algorithm::simplify(_ring,_simpleRing,_limit);
  auto _it = _simpleRing.begin();
  RING _junction;
  _junction.reserve(48);

  gex::Polygon _connectionPolygon;
  _connectionPolygon.boundary().reserve(_ring.size() * 4);

  OUTPUT _unionOutput;
  _unionOutput.reserve(2);

  while (1)
  {
    if (_it == _simpleRing.end()-1) break;

    auto _p0 = *_it;
    point_type _p1, _p2;
    if (_it == _simpleRing.end()-2)
    {
      _p1 = _simpleRing.front();
      _p2 = _simpleRing[1];
    }
    else if (_it == _simpleRing.end()-3)
    {
      _p1 = *(_simpleRing.end()-2);
      _p2 = _simpleRing.front();
    }
    else
    {
      _p1 = *(_it+1);
      _p2 = *(_it+2);
    }

    using namespace gex::algorithm::functor;

    generateJunction(_p0,_p1,_p2,_offset,_limit,_junction,_svg);
    GEX_ASSERT(!_junction.empty());
    if (!_junction.empty())
    {
      gex::algorithm::correct(_junction);
      _svg.draw(_junction,"stroke:white;stroke-width:0.5;fill:none");
      if (_connectionPolygon.boundary().empty())
      {
        _connectionPolygon.boundary() = _junction;
      }
      else
      {
        for (int _attempts = 0; _attempts < 3; ++_attempts)
        {
          gex::algorithm::union_(_junction,_connectionPolygon,_unionOutput);
          //    std::cout << _unionOutput.size() << std::endl;
          if (_unionOutput.size() == 1)
          {
            _connectionPolygon =_unionOutput[0];
            _unionOutput.clear();
            break;
          }
          else if (_unionOutput.empty())
          {
for (auto& _p : _junction)
              _p += point_type(_limit,_limit);
          }
        }

        gex::algorithm::correct(_connectionPolygon);
        _unionOutput.clear();
      }
    }
    ++_it;
  }

  if (_offset > 0)
  {
    _output.push_back(_connectionPolygon.boundary());
for (auto& _hole : _connectionPolygon.holes())
      if (gex::algorithm::disjoint(_hole,_ring))
        _output.back().holes().emplace_back(_hole);
  }
  else
  {
for (auto& _hole : _connectionPolygon.holes())
      if (gex::algorithm::within(_hole,_simpleRing))
        _output.emplace_back(_hole);
  }
}





int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  //_circle = create::irregular(gex::Point2(),5,false,16);
  _circle = create::irregular(gex::Point2(),6,false,16);

  //_circle.pop_back();
  boost::geometry::correct(_circle);

  gex::MultiPolygon _offsetCircle;

  _svg.buffer().fit(_bounds);

  int _index = 0;
  for (double _offset = -10.0 ; _offset <= 10.0; _offset += 0.05)
  {
    gex::Ring _out;
    _offsetCircle.clear();
    _svg.clear();
    auto _limit = _circle.bounds().size().length() / 10000;
    /*
        for (int i = 0; i < _circle.size()-2; i++)
        {
          generateJunction(_circle[i+0],_circle[i+1],_circle[i+2],_offset,_limit,_out,_svg);

          _svg.draw(_circle,"stroke:red;fill:none");

          if (!_out.empty())
            _svg.draw(_out,"stroke:blue;fill:none");

          std::stringstream ss;
          ss << "offset_" << _offset << "_" << i << ".svg";
          _svg.buffer().write(ss.str());
        }
    */
    _svg.clear();
    _svg.draw(_circle,"stroke:red;fill:none");

    rawOffsetRing(_circle,_offset,_limit,_out,_svg);

    //gex::Ring _bounds = create::circle(gex::Point2(0,0),10 + _offset);
    //gex::algorithm::intersection(_out,_bounds,_offsetCircle);



  //  _svg.draw(_out,"stroke:lime;stroke-width:4;fill:none");
    //  _offsetCircle.clear();
  //    gex::algorithm::offset(_circle,_offsetCircle,_offset);
    offsetRing(_circle,_offset,_offsetCircle,_svg);
    _svg.draw(_offsetCircle,"stroke:orange;stroke-width:3;fill:none");
    std::stringstream ss;
    ss << "offset_" << std::setw(5) << std::setfill('0') << _index << ".svg";
    _svg.buffer().write(ss.str());
    ++_index;
  }

  return EXIT_SUCCESS;
}

