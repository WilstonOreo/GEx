#include "create.hpp"
#include <gex/algorithm/util.hpp>
#include <gex/algorithm/reverse.hpp>

#include <gex/algorithm/for_each.hpp>

namespace gex
{
  namespace base
  {
    template<typename POINT>
    struct Curve
    {
      typedef POINT point_type;
      typedef std::array<point_type,3> points_type;

      Curve(const POINT& _p0, const POINT& _p1, const POINT& _p2)
      {
        points_[0] = _p0;
        points_[1] = _p1;
        points_[2] = _p2;
      }

      void determinant() const
      {
        auto& _a = points_[0];
        auto& _b = points_[1];
        auto& _c = points_[2];
        return (_b.x() * _c.y() + _a.x() * _b.y() + _a.y()*_c.x()) -
               (_a.y() * _b.x() + _b.y() * _c.x() + _a.x()*_c.y());
      }

      void angle() const
      {
        auto& _a = points_[0];
        auto& _b = points_[1];
        auto& _c = points_[2];
        auto&& _v0 = (_c - _b).normalized();
        auto&& _n = _a - _b;
        _n(-_n.y(),_n.x());
        _n.normalize();
        return dot(_n,_v0);
      }

      TBD_PROPERTY_REF_RO(points_type,points)
    };
  }
}

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT>
      struct ForEach<base::Curve<POINT>,prim::LineString<POINT>,true>
      {
        typedef base::Curve<POINT> curve_type;
        typedef prim::LineString<POINT> primitive_type;

        template<typename FUNCTOR>
        void operator()(const primitive_type& _lineString, FUNCTOR f)
        {
          size_t _i = 0, _n = _lineString.size()-2;
          while (_i < _n)
          {
            auto& _p0 = _lineString[_i];
            auto& _p1 = _lineString[_i+1];
            auto& _p2 = _lineString[_i+2];
            f(curve_type(_p0,_p1,_p2));
            ++_i;
          }
        }
      };

      template<typename POINT>
      struct ForEach<base::Curve<POINT>,prim::Ring<POINT>,true>
      {
        typedef base::Curve<POINT> curve_type;
        typedef prim::Ring<POINT> primitive_type;

        template<typename FUNCTOR>
        void operator()(const primitive_type& _ring, FUNCTOR f)
        {
        
          size_t _i = 0, _n = _ring.size();
          if (_n < 3) return;
          while (_i < _n-3)
          {
            auto& _p0 = _ring[_i];
            auto& _p1 = _ring[_i+1];
            auto& _p2 = _ring[_i+2];
            f(curve_type(_p0,_p1,_p2));
            ++_i;
          }
          f(curve_type(_ring[_n-3],_ring[_n-2],_ring[0]));
          f(curve_type(_ring[_n-2],_ring[0],_ring[1]));
        }
      };
    }
  }
}

#include <gex/io.hpp>
#include <gex/algorithm/offset.hpp>

/*
#include <gex/algorithm/union.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/unify.hpp>
#include <boost/geometry/algorithms/equals.hpp>


namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINT, typename RING>
        void generateJunction( const POINT& _p0,
                               const POINT& _p1,
                               const POINT& _p2,
                               typename POINT::Scalar _offset,
                               typename POINT::Scalar _limit,
                               RING& _ring,
                               gex::io::SVG& _svg
                             )
        {
          typedef POINT point_type;
          typedef typename POINT::Scalar scalar_type;
          typedef typename RING::vec_type vec_type;

          _ring.clear();
          if ((_p0 - _p1).norm() < _limit)
            return;

          using namespace algorithm::util;

          // Normals
          auto _n0 = getNormal(_p0,_p1).normalized(),
               _n1 = getNormal(_p1,_p2).normalized();

          // Scaled normals;
          vec_type _sn0(_n0);
          _sn0 *= _offset;
          vec_type _sn1(_n1);
          _sn1 *= _offset;

          auto _front = _p0;
          auto _back = (_p2 - _p1).normalized() * _offset + _p1;

          _ring.clear();
          _ring.push_back(_front - _sn0);
          _ring.push_back(_front + _sn0);
          double _a0 = std::atan2(_n1.y(),_n1.x()),
                 _a1 = std::atan2(_n0.y(),_n0.x());

          if (_a1 < _a0) _a1 += M_PI * 2.0;

          if (_a1 - _a0 < M_PI)
          {
            detail::buildArc(_p1,_a1,_a0,_offset,_limit,_ring);
          }
          else
          {
            gex::prim::Segment<POINT> _seg0(_front + _sn0,
                                            _p1 + _sn0),
                                                _seg1(_p1 + _sn1,
                                                    _back + _sn1);
            POINT _iPoint;
            if (lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            } else
            {
              _ring.push_back(_p1 + _sn0);
            }
          }
      
         
          if (_a1 - _a1 > M_PI)
          _ring.push_back(_back);

          _a0 += M_PI;
          _a1 -= M_PI;

          if (_a0 - _a1 < M_PI)
          {
            detail::buildArc(_p1,_a0,_a1,_offset,_limit,_ring);
          }
          else
          {
            gex::prim::Segment<POINT>
            _seg0(_front - _sn0,_p1 - _sn0),
                  _seg1(_p1 - _sn1,_back - _sn1);
            POINT _iPoint;
            if (lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            } else
            {
              _ring.push_back(_p1 - _sn0);
            }
          }

          _ring.update();
        }


        template<typename RING, typename OFFSET, typename LIMIT, typename POLYGON>
        bool generateOffsetPolygon(
          const RING& _ring,
          const OFFSET& _offset,
          const LIMIT& _limit,
          POLYGON& _polygon,
          gex::io::SVG& _svg)
        {
          RING _junction;
          typedef typename RING::point_type point_type;
          typedef typename RING::scalar_type scalar_type;
          typedef base::Curve<point_type> curve_type;
          typedef gex::prim::MultiPolygon<POLYGON> multipolygon_type;
          _junction.reserve(64);
          multipolygon_type _offsetPolygons;

          bool _skip = false;
          gex::for_each<curve_type>(_ring,[&](const curve_type& _curve)
          {
            if (_skip) return;
            _offsetPolygons.clear();
            gex::algorithm::functor::detail::generateJunction(
              _curve.points()[0],
              _curve.points()[1],
              _curve.points()[2],
              _offset,_limit,_junction,_svg);
            
            scalar_type _radius = 0;
            boost::geometry::correct(_junction);
            gex::algorithm::union_(_polygon,_junction,_offsetPolygons);              
            
            if (_offsetPolygons.empty())
            {
              _skip = true;
              return;
            } 
            _polygon = _offsetPolygons[0];
            _polygon.update();
          });

          if (_skip) 
            return false;

          typedef gex::prim::LineString<point_type> linestring_type;
          if (intersects(
              linestring_type(_ring.begin(),_ring.end()),
              linestring_type(_polygon.boundary().begin(),_polygon.boundary().end())))
          {
            return false;
          }
          return true;
        }
      }
    }
  }
}*/


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
      _curve.points()[0],
      _curve.points()[1],
      _curve.points()[2],
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
