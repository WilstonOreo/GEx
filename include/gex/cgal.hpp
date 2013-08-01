#pragma once

#include "prim.hpp"

#include <CGAL/assertions.h>
#include <CGAL/Origin.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

namespace gex
{
  namespace cgal
  {
    //typedef CGAL::Exact_predicates_inexact_constructions_kernel Traits;
    typedef CGAL::Simple_cartesian<Scalar> Traits;
    typedef CGAL::Polygon_2<Traits> Ring;
    typedef CGAL::Polygon_with_holes_2<Traits> Polygon;
    typedef CGAL::Point_2<Traits> Point2;
    typedef boost::shared_ptr<Ring> PolygonPtr;
    typedef std::vector<PolygonPtr> PolygonPtrVector;

    template<typename PRIMITIVE>
    struct AdaptType
    {
    };

#define GEX_CGAL_ADAPT_TYPE(in_type,out_type)\
      template<> struct AdaptType<in_type>\
      {\
        typedef out_type type;\
      };

    GEX_CGAL_ADAPT_TYPE(gex::Point2,cgal::Point2)
    GEX_CGAL_ADAPT_TYPE(cgal::Point2,gex::Point2)
    GEX_CGAL_ADAPT_TYPE(gex::Ring,cgal::Ring)
    GEX_CGAL_ADAPT_TYPE(cgal::Ring,gex::Ring)
    GEX_CGAL_ADAPT_TYPE(gex::Polygon,cgal::Polygon)
    GEX_CGAL_ADAPT_TYPE(cgal::Polygon,gex::Polygon)
    GEX_CGAL_ADAPT_TYPE(PolygonPtrVector,gex::MultiPolygon)
    GEX_CGAL_ADAPT_TYPE(gex::MultiPolygon,std::vector<cgal::Polygon>)

    namespace
    {
      template<typename PRIMITIVE>
      struct Adapter {};
    }

    template<typename PRIMITIVE>
    void adapt(const PRIMITIVE& _in, typename AdaptType<PRIMITIVE>::type& _out)
    {
      Adapter<PRIMITIVE>()(_in,_out);
    }

    template<typename PRIMITIVE>
    typename AdaptType<PRIMITIVE>::type adapt(const PRIMITIVE& _in)
    {
      typename AdaptType<PRIMITIVE>::type _out;
      adapt<PRIMITIVE>(_in,_out);
      return _out;
    }

    namespace
    {
#define GEX_CGAL_ADAPTER(primitive)\
      template<>\
      struct Adapter<primitive>\
      {\
        typedef primitive in_type;\
        typedef AdaptType<primitive>::type out_type;\
        inline void operator()(const in_type&, out_type&);\
      };\
      void Adapter<primitive>::operator()(const in_type& _in, out_type& _out)

      GEX_CGAL_ADAPTER(gex::Point2)
      {
        _out = out_type(_in.x(),_in.y());
      }

      GEX_CGAL_ADAPTER(cgal::Point2)
      {
        typedef out_type::scalar_type scalar_type;
        _out = out_type(scalar_type(_in.x()),scalar_type(_in.y()));
      }

      /// Convert a gex::Ring to a gex::cgal::Ring
      GEX_CGAL_ADAPTER(gex::Ring)
      {
        _out.clear();
        for (auto it = _in.begin() ; it != --_in.end() ; ++it)
          _out.push_back(adapt(*it));
      }

      /// Convert a gex::cgal::Ring to a gex::Ring
      GEX_CGAL_ADAPTER(cgal::Ring)
      {
        _out.clear();
        _out.reserve(_in.container().size());
for (const auto& p : _in.container())
          _out.push_back(adapt(p));
        _out.update();
      }

      /// Convert a gex::Ring to a gex::cgal::Polygon
      GEX_CGAL_ADAPTER(gex::Polygon)
      {
        _out = cgal::Polygon(adapt(_in.boundary()));
for (auto& _hole : _in.holes())
          _out.add_hole(adapt(_hole));
      }

      /// Convert a gex::cgal::Polygon to a gex::Polygon
      GEX_CGAL_ADAPTER(cgal::Polygon)
      {
        _out.boundary() = adapt(_in.outer_boundary());
        _out.holes().clear();
        for (auto it = _in.holes_begin(); it != _in.holes_end(); ++it)
        {
          _out.holes().push_back(adapt(*it));
        }
        _out.update();
      }

      GEX_CGAL_ADAPTER(cgal::PolygonPtrVector)
      {
        _out.clear();
for (auto _polygonPtr : _in)
        {
          _out.push_back(adapt(*_polygonPtr));
        }
      }

      GEX_CGAL_ADAPTER(gex::MultiPolygon)
      {
        _out.clear();
for (auto& _p : _in)
          _out.push_back(adapt(_p));
      }
    }
  }
}
