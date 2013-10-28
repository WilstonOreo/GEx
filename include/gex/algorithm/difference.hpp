#pragma once

#include <boost/geometry/algorithms/difference.hpp>
#include <gex/prim.hpp>
#include <gex/polygon.hpp>

namespace gex
{
  namespace algorithm
  {
    template<typename A, typename B>
    struct DifferenceType {};

#define GEX_DIFFERENCE_TYPE(a,b,i) \
        template<> struct DifferenceType<a,b> { typedef i type; };

    GEX_DIFFERENCE_TYPE(Ring,Ring,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Ring,Polygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Ring,MultiPolygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Polygon,MultiPolygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Polygon,Polygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Polygon,Ring,MultiPolygon)
    GEX_DIFFERENCE_TYPE(MultiPolygon,Ring,MultiPolygon)
    GEX_DIFFERENCE_TYPE(MultiPolygon,Polygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(MultiPolygon,MultiPolygon,MultiPolygon)
    GEX_DIFFERENCE_TYPE(Range,Range,Range)

    GEX_DIFFERENCE_TYPE(OrthogonalPlane<X>,Triangle,Segment)
    GEX_DIFFERENCE_TYPE(OrthogonalPlane<Y>,Triangle,Segment)
    GEX_DIFFERENCE_TYPE(OrthogonalPlane<Z>,Triangle,Segment)

    GEX_DIFFERENCE_TYPE(polygon::Ring,polygon::Ring,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::Ring,polygon::Polygon,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::Ring,polygon::MultiPolygon,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::Polygon,polygon::Ring,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::Polygon,polygon::Polygon,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::Polygon,polygon::MultiPolygon,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::MultiPolygon,polygon::Ring,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::MultiPolygon,polygon::Polygon,polygon::MultiPolygon)
    GEX_DIFFERENCE_TYPE(polygon::MultiPolygon,polygon::MultiPolygon,polygon::MultiPolygon)

    template<typename A, typename B>
    struct Difference
    {
      template<typename I>
      void operator()(const A& _a, const B& _b, I& _i)
      {
        try
        {
          boost::geometry::difference(_a,_b,_i);
        }
        catch (std::exception e)
        {
          _i = I();
          std::cerr << "Warning: boost::geometry::difference failed due to numerical inconsistencies." << std::endl;
        }
      }
    };

    /// Boost Polygon operations
    template<>
    struct Difference<polygon::Polygon,polygon::Polygon>
    {
      template<typename A, typename B, typename I>
      inline void operator()(const A& _a, const B& _b, I& _i)
      {
        using namespace boost::polygon::operators;
        _i = _a - _b;
      }
    };

    template<>
    struct Difference<polygon::Ring,polygon::Ring> :
        Difference<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Difference<polygon::Ring,polygon::Polygon> :
        Difference<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Difference<polygon::Ring,polygon::MultiPolygon> :
        Difference<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Difference<polygon::Polygon,polygon::Ring> :
        Difference<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Difference<polygon::Polygon,polygon::MultiPolygon> :
        Difference<polygon::Polygon,polygon::Polygon> {};
    
    template<>
    struct Difference<polygon::MultiPolygon,polygon::Ring> :
        Difference<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Difference<polygon::MultiPolygon,polygon::Polygon> :
        Difference<polygon::Polygon,polygon::Polygon> {};

    template<>
    struct Difference<polygon::MultiPolygon,polygon::MultiPolygon> :
        Difference<polygon::Polygon,polygon::Polygon> {};

    template<typename A, typename B, typename I>
    void difference(const A& _a, const B& _b, I& _i)
    {
      Difference<A,B>()(_a,_b,_i);
    }

    template<typename A, typename B>
    typename DifferenceType<A,B>::type difference(const A& _a, const B& _b)
    {
      typename DifferenceType<A,B>::type _i;
      difference(_a,_b,_i);
      return _i;
    }

    template<typename A, typename B>
    typename DifferenceType<A,B>::type operator&(const A& _a, const B& _b)
    {
      return difference(_a,_b);
    }
  }

  using algorithm::difference;
}

