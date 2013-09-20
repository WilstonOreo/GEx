#pragma once

#include "boost/geometry/algorithms/union.hpp"
#include "gex/prim.hpp"

namespace gex
{
  namespace algorithm
  {
    template<typename A, typename B>
    struct UnionType
    {
      typedef A type;
    };

#define GEX_GEOMETRY_UNION_TYPE(a,b,i) \
      template<> struct UnionType<a,b> { typedef i type; };

    GEX_GEOMETRY_UNION_TYPE(Ring,Ring,std::vector<Ring>)
    GEX_GEOMETRY_UNION_TYPE(Ring,Polygon,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(Ring,MultiPolygon,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(Polygon,MultiPolygon,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(Polygon,Polygon,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(Polygon,Ring,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(MultiPolygon,Ring,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(MultiPolygon,Polygon,MultiPolygon)
    GEX_GEOMETRY_UNION_TYPE(MultiPolygon,MultiPolygon,MultiPolygon)

    template<typename A, typename B>
    struct Union
    {
      template<typename I>
      void operator()(const A& _a, const B& _b, I& _i)
      {
        try
        {
          boost::geometry::union_(_a,_b,_i);
        }
        catch (std::exception e)
        {
          _i = I();
          std::cerr << "Warning: boost::geometry::union_ failed due to numerical inconsistencies." << std::endl;
        }
      }
    };

    template<typename SCALAR>
    struct Union<base::Range<SCALAR>,base::Range<SCALAR>>
    {
      typedef base::Range<SCALAR> range_type;

      void operator()(const range_type& _a, const range_type& _b, range_type& _i)
      {
        _i(std::min(_a.min(),_b.min()),std::max(_a.max(),_b.max()));
      }
    };

    template<typename A, typename B, typename I>
    void union_(const A& _a, const B& _b, I& _i)
    {
      Union<A,B>()(_a,_b,_i);
    }

    template<typename A, typename B>
    typename UnionType<A,B>::type union_(const A& _a, const B& _b)
    {
      typename UnionType<A,B>::type _i;
      union_<A,B>(_a,_b,_i);
      return _i;
    }

    template<typename A, typename B>
    typename UnionType<A,B>::type operator|(const A& _a, const B& _b)
    {
      return union_(_a,_b);
    }
  }
  using algorithm::union_;
}
