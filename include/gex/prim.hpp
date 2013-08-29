#pragma once

#include "base.hpp"

#include "prim/Vertex.hpp"
#include "prim/Segment.hpp"
#include "prim/LineString.hpp"
#include "prim/Ring.hpp"
#include "prim/Polygon.hpp"
#include "prim/MultiPolygon.hpp"
#include "prim/Triangle.hpp"
#include "prim/BoundingBox.hpp"
#include "prim/Sphere.hpp"
#include "prim/OrthogonalPlane.hpp"
#include "prim/MultiPoint.hpp"

namespace gex
{
    typedef prim::Segment<Point2> Segment;
    typedef prim::LineString<Point2> LineString;
    typedef prim::MultiLineString<Point2> MultiLineString;
    typedef prim::Vertex<Model2> Vertex2;
    typedef prim::Ring<Point2> Ring;
    typedef prim::MultiRing<Point2> MultiRing;
    typedef prim::Polygon<Ring> Polygon;
    typedef prim::MultiPolygon<Polygon> MultiPolygon;

    typedef prim::MultiPoint<Point2> MultiPoint2;
    typedef prim::MultiPoint<Point3> MultiPoint3;

    typedef prim::MultiSegment<Point2> MultiSegment;

    typedef prim::Vertex<Model3> Vertex3;
    typedef prim::BoundingBox<Model3> BoundingBox;
    typedef prim::Triangle<Point3> Triangle;
    typedef prim::Sphere<Scalar> Sphere;  

    template<base::Axis AXIS>
    using OrthogonalPlane = prim::OrthogonalPlane<Scalar,AXIS>;
}

#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/register/segment.hpp>
BOOST_GEOMETRY_REGISTER_SEGMENT(gex::Segment,gex::Point2,p0(),p1())

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
BOOST_GEOMETRY_REGISTER_LINESTRING(gex::LineString)

#include <boost/geometry/multi/geometries/multi_linestring.hpp>
#include <boost/geometry/multi/geometries/register/multi_linestring.hpp>
BOOST_GEOMETRY_REGISTER_MULTI_LINESTRING(gex::MultiLineString)

#include <boost/geometry/geometries/register/ring.hpp>
BOOST_GEOMETRY_REGISTER_RING(gex::Ring)

#include <boost/geometry/geometries/polygon.hpp>
/// Boost Geometry Polygon Type Registration
namespace boost
{
  namespace geometry
  {
    namespace traits
    {
      using gex::Ring;
      using gex::Polygon;

      template<>
      struct tag<Polygon>
      {
        typedef polygon_tag type;
      };

      template<>
      struct ring_const_type<Polygon>
      {
        typedef Ring const& type;
      };
      
      template<>
      struct ring_mutable_type<Polygon>
      {
        typedef Ring& type;
      };

      template<>
      struct interior_const_type<Polygon>
      {
        typedef std::vector<Ring> const& type;
      };

      template<>
      struct interior_mutable_type<Polygon>
      {
        typedef std::vector<Ring>& type;
      };

      template<>
      struct exterior_ring<Polygon>
      {
        static Ring& get(Polygon& p)
        {
          return p.boundary();
        }

        static Ring const& get(Polygon const& p)
        {
          return p.boundary();
        }
      };

      template<>
      struct interior_rings<Polygon>
      {
        typedef std::vector<Ring> holes_type;

        static holes_type& get(Polygon& p)
        {
          return p.holes();
        }

        static holes_type const& get(Polygon const& p)
        {
          return p.holes();
        }
      };
    }
  }
} // namespace boost::geometry::traits
 
#include <boost/geometry/multi/geometries/register/multi_polygon.hpp>
BOOST_GEOMETRY_REGISTER_MULTI_POLYGON(gex::MultiPolygon)
