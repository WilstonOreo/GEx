#include <gex/algorithm/offset.hpp>

#include <boost/geometry/extensions/algorithms/dissolve.hpp>

#include <gex/io/SVG.hpp>
#include <gex/io/read.hpp>

typedef gex::base::Model<2,int> Model2i;
typedef gex::base::Point<Model2i> Point2i;
typedef gex::prim::Ring<Point2i> Ring2i;
typedef gex::prim::LineString<Point2i> LineString2i;
typedef gex::prim::MultiLineString<Point2i> MultiLineString2i;
typedef gex::prim::Polygon<Ring2i> Polygon2i;
typedef gex::prim::MultiPolygon<Polygon2i> MultiPolygon2i;

static inline size_t size() { return 1 << 15; }

template<typename POINT>
POINT transform(const POINT& _p, const gex::Bounds2& _bounds)
{
  POINT _tP = _p; // Transformed point
  _tP -= _bounds.min();
  auto&& _size = _bounds.size();
  _tP[0] *= size() / _size.x();
  _tP[1] *= size() / _size.y();
  return _tP;
}

template<typename POINT>
POINT invTransform(const POINT& _p, const gex::Bounds2& _bounds)
{
  POINT _tP = _p; // Transformed point
  auto&& _size = _bounds.size();
  _tP[0] *= _size.x() / size();
  _tP[1] *= _size.y() / size();
  _tP += _bounds.min();
  return _tP;
}

static inline Point2i adapt(const gex::Point2& _pIn, const gex::Bounds2& _bounds)
{
  auto&& _tP = transform(_pIn,_bounds);
  return Point2i(_tP.x(),_tP.y());
}

static inline gex::Point2 adapt(const Point2i& _pIn, const gex::Bounds2& _bounds)
{
  gex::Point2 _tP(_pIn.x(),_pIn.y());
  return invTransform(_tP,_bounds);
}

static inline Ring2i adapt(const gex::Ring& _in, const gex::Bounds2& _bounds)
{
  Ring2i _out;
  gex::for_each<gex::Point2>(_in,[&](const gex::Point2& _p)
  {
    _out.push_back(adapt(_p,_bounds));
  });
  return _out;
}

static inline gex::Ring adapt(const Ring2i& _in, const gex::Bounds2& _bounds)
{
  gex::Ring _out;
  gex::for_each<Point2i>(_in,[&](const Point2i& _p)
  {
    _out.push_back(adapt(_p,_bounds));
  });
  return _out;
}

static inline LineString2i adapt(const gex::LineString& _in, const gex::Bounds2& _bounds)
{
  LineString2i _out;
  gex::for_each<gex::Point2>(_in,[&](const gex::Point2& _p)
  {
    _out.push_back(adapt(_p,_bounds));
  });
  return _out;
}

static inline gex::LineString adapt(const LineString2i& _in, const gex::Bounds2& _bounds)
{
  gex::LineString _out;
  gex::for_each<Point2i>(_in,[&](const Point2i& _p)
  {
    _out.push_back(adapt(_p,_bounds));
  });
  return _out;
}


GEX_REGISTER_POINT(Point2i,2,int)

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
BOOST_GEOMETRY_REGISTER_LINESTRING(LineString2i)

#include <boost/geometry/multi/geometries/multi_linestring.hpp>
#include <boost/geometry/multi/geometries/register/multi_linestring.hpp>
BOOST_GEOMETRY_REGISTER_MULTI_LINESTRING(MultiLineString2i)

#include <boost/geometry/geometries/register/ring.hpp>
BOOST_GEOMETRY_REGISTER_RING(Ring2i)

#include <boost/geometry/geometries/polygon.hpp>
/// Boost Geometry Polygon Type Registration
namespace boost
{
  namespace geometry
  {
    namespace traits
    {
      template<>
      struct tag<Polygon2i>
      {
        typedef polygon_tag type;
      };

      template<>
      struct ring_const_type<Polygon2i>
      {
        typedef Ring2i const& type;
      };
      
      template<>
      struct ring_mutable_type<Polygon2i>
      {
        typedef Ring2i& type;
      };

      template<>
      struct interior_const_type<Polygon2i>
      {
        typedef std::vector<Ring2i> const& type;
      };

      template<>
      struct interior_mutable_type<Polygon2i>
      {
        typedef std::vector<Ring2i>& type;
      };

      template<>
      struct exterior_ring<Polygon2i>
      {
        static Ring2i& get(Polygon2i& p)
        {
          return p.boundary();
        }

        static Ring2i const& get(Polygon2i const& p)
        {
          return p.boundary();
        }
      };

      template<>
      struct interior_rings<Polygon2i>
      {
        typedef std::vector<Ring2i> holes_type;

        static holes_type& get(Polygon2i& p)
        {
          return p.holes();
        }

        static holes_type const& get(Polygon2i const& p)
        {
          return p.holes();
        }
      };
    }
  }
} // namespace boost::geometry::traits
 
#include <boost/geometry/multi/geometries/register/multi_polygon.hpp>
BOOST_GEOMETRY_REGISTER_MULTI_POLYGON(MultiPolygon2i)


template<typename LINESTRING>
void examineLineString(const LINESTRING& _lineString)
{
  gex::for_each<gex::Segment>(_lineString,[&](const gex::Segment& _seg)
  {
    std::cout << gex::distance(_seg.p0(),_seg.p1()) << std::endl;
  });
}


int main(int argc, char* argv[])
{
  if (argc < 2) return EXIT_FAILURE;

  using namespace gex;

  std::string _wkt(argv[1]);

  MultiPolygon _polygons;
  MultiPolygon _offsetPolygons;
  io::readWkt(_wkt,_polygons);

  std::vector<Scalar> _offsets = { -1.3,-0.9,-0.5,-0.01, 0.01, 0.05, 0.1, 0.3, 0.5, 0.9, 1.3 };

  size_t _number = 0, _empty = 0;
  for (auto& _polygon : _polygons)
  {
    io::SVG _svg;
    _polygon.update(); 

    for_each<Ring>(_polygon,[&](const Ring& _ring)
    {
    //  Ring2i _r = adapt(_ring,_bounds);
    
      auto& _bounds = _ring.bounds();
     // auto&& _ls = algorithm::convert<LineString>(_ring);
      Ring _lineString;
      boost::geometry::simplify(_ring,_lineString,_bounds.size().norm() / 1000.0);

      //_lineString.pop_back();

      boost::geometry::correct(_lineString);

   //   std::cout << _lineString.size() << std::endl;

      for (auto& _offset : _offsets)
      {     
        std::stringstream _ss;
        _ss << "offset_test/" << std::setw(5) << std::setfill('0') << _number << "_" << _offset;
        _offsetPolygons.clear();
        gex::offset(_lineString,_offset,_offsetPolygons);
      //  std::cout << _ss.str() << "\t" << _offset;

        if (_offsetPolygons.empty())
        {
          _svg.clear();
          auto&& _center = _bounds.center();
          auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
          Vec2 _m(_max,_max);
          _svg.buffer().fit(Bounds2(_center - _m,_center + _m));
          _svg.draw(_lineString,"stroke:red;fill:none");
          _svg.draw(_offsetPolygons,"stroke:green;fill:none");
          
          examineLineString(_lineString);
          _ss << "_defect";
          std::cout << "\tIS EMPTY!!!\t";
          std::cout << std::endl;
          _svg.buffer().write(_ss.str()+"_ring.svg");
          _empty++;
        }
          

        _number++;
      }
    });


/*

    auto _polyArea = boost::geometry::area(_polygon);


    _svg.draw(_polygon,"stroke:red;fill:none");

    using algorithm::offset;

    bool _previousEmpty = true;


      /*
            _offsetPolygons.clear();
            offset(_polygon,_offset,_offsetPolygons);
            _offsetPolygons.update();
            auto _offArea = boost::geometry::area(_offsetPolygons);


            std::cout << _offset << "\t" << _offArea << "\t" << _polyArea << std::endl;



            bool _assert = (_offArea <= _polyArea && _offset > 0) || (_offArea >= _polyArea && _offset < 0);
            if (_assert)
            {
              _svg.buffer().write(_ss.str()+"_polygon_assert.svg");
              GEX_ASSERT(_assert);
            }
        */


  }
    std::cout << _empty << " of " << _number << " (" << float(_empty*100)/_number << "%) are empty." << std::endl;

  return EXIT_SUCCESS;
}
