#pragma once

#include "base.hpp"

#include "prim/Primitive.hpp"
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

namespace gex
{
    typedef prim::Primitive<Model2> Primitive2;
    typedef prim::Segment<Model2> Segment;
    typedef prim::LineString<Model2> LineString;
    typedef prim::MultiLineString<Model2> MultiLineString;
    typedef prim::Vertex<Model2> Vertex2;
    typedef prim::Ring<Model2> Ring;
    typedef prim::Polygon<Model2> Polygon;
    typedef prim::MultiPolygon<Model2> MultiPolygon;

    typedef prim::Primitive<Model3> Primitive3;
    typedef prim::Triangle<Model3> Triangle;
    typedef prim::Vertex<Model3> Vertex3;
    typedef prim::BoundingBox<Model3> BoundingBox;
    typedef prim::Sphere<Model3> Sphere;  

    template<base::Axis AXIS>
    using OrthogonalPlane = prim::OrthogonalPlane<Model3,AXIS>;
}

#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/register/segment.hpp>
BOOST_GEOMETRY_REGISTER_SEGMENT(gex::Segment,gex::Point2,p0,p1)

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
