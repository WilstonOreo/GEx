#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/util.hpp>
#include <gex/algorithm/functor/detail/generateJunction.hpp>
#include <gex/io.hpp>

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

    if ((_p0 - _p1).norm() < _limit)
      return;

    using namespace gex::algorithm;
    auto _n0 = util::getNormal(_p0,_p1).normalized(),
         _n1 = util::getNormal(_p1,_p2).normalized();

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
      functor::detail::buildArc(_p1,_a1,_a0,_offset,_limit,_out);
    }
    else
    {
      gex::Segment _seg0(_p0 + _offset * _n0,
                         _p1 + _offset * _n0),
                             _seg1(_p1 + _offset * _n1,
                                   _p2 + _offset * _n1);
   //   _svg.draw(_seg0,"stroke:orange;fill:none");
   //   _svg.draw(_seg1,"stroke:yellow;fill:none");

      gex::Point2 _iPoint;
      if (util::lineSegmentIntersection(_seg0.p0(),_seg0.p1(),_seg1.p0(),_seg1.p1(),_iPoint))
      {
        //_svg.draw(_seg0,"stroke:orange;fill:none");
     //   _svg.draw(_iPoint,"stroke:green;fill:none");
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

  if (std::abs(_offset) <= _eps)
  {
    _output.emplace_back(_ring);
    return;
  }

  auto _limit = _ring.bounds().size().norm() * _eps;

  RING _simpleRing;
  gex::algorithm::simplify(_ring,_simpleRing,_limit);
  auto _it = _simpleRing.begin();
  RING _junction;
  _junction.reserve(48);

  gex::Polygon _connectionPolygon;
  _connectionPolygon.boundary().reserve(_ring.size() * 4);

  OUTPUT _unionOutput, _failed;
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

    detail::generateJunction(_p0,_p1,_p2,_offset,_limit,_junction);
//    GEX_ASSERT(!_junction.empty());
    if (!_junction.empty())
    {
      _svg.draw(_junction,"stroke:white;stroke-width:0.5;fill:none");
      if (_connectionPolygon.boundary().empty())
      {
        _connectionPolygon.boundary() = _junction;
      }
      else
      {
        _connectionPolygon.update();
        gex::algorithm::correct(_connectionPolygon);
        gex::algorithm::union_(_junction,_connectionPolygon,_unionOutput);
        if (_unionOutput.size() == 1)
        {
          _connectionPolygon =_unionOutput[0];
          _unionOutput.clear();
        } else
        {
          _svg.draw(_junction,"stroke:fuchsia;stroke-width:2;fill:none");
        }
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
  //_circle = create::irregular(gex::Point2(0.0,0.0),10,false,16);
  _circle = create::star(gex::Point2(0.0,0.0),3,7,false,40);

  //_circle.pop_back();
  boost::geometry::correct(_circle);

  gex::MultiPolygon _offsetCircle;

  std::vector<double> _offsets;
  const int _num = 50;
  for (int i = 0; i < _num; ++i)
  {
    double _offset = 4*(i-_num/2)/(double)_num;
    _offset = _offset*_offset*_offset;
    std::cout << _offset << std::endl;
    _offsets.push_back(_offset);
  }

  auto _tmp = _offsets;
  std::reverse(_tmp.begin(),_tmp.end());
  _offsets.insert(_offsets.end(),_tmp.begin(),_tmp.end());


  int _index = 0;
  for (double _offset : _offsets)
  {
    gex::Ring _out;
    _offsetCircle.clear();
    _svg.clear(512,512);
    _svg.buffer().fit(_bounds);
    auto _limit = _circle.bounds().size().norm() / 10000;
    
    for (int i = 0; i < _circle.size()-2; i++)
    {
      gex::algorithm::functor::detail::generateJunction(_circle[i+0],_circle[i+1],_circle[i+2],_offset,_limit,_out);

      _svg.draw(_circle,"stroke:red;fill:none");

      if (!_out.empty())
        _svg.draw(_out,"stroke:blue;fill:none");

      std::stringstream ss;
      ss << "offset_" << _offset << "_" << i << ".svg";
      _svg.buffer().write(ss.str());
    }
    
    /*
    _svg.clear();
    _svg.draw(_circle,"stroke:red;fill:none");

  //  rawOffsetRing(_circle,_offset,_limit,_out,_svg);
    _svg.draw(_out,"stroke:blue;stroke-width:0.5;fill:none");

    //gex::Ring _bounds = create::circle(gex::Point2(0,0),10 + _offset);
    //gex::algorithm::intersection(_out,_bounds,_offsetCircle);



  //  _svg.draw(_out,"stroke:lime;stroke-width:4;fill:none");
    //  _offsetCircle.clear();
  //    gex::algorithm::offset(_circle,_offsetCircle,_offset);
    offsetRing(_circle,_offset,_offsetCircle,_svg);
    _svg.draw(_offsetCircle,"stroke:yellow;stroke-width:3;fill:none");*/
    std::stringstream ss;
    ss << "offset_" << std::setw(5) << std::setfill('0') << _index << ".svg";
    _svg.buffer().write(ss.str());
    ++_index;
  }

  return EXIT_SUCCESS;
}

