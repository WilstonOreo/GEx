#pragma once
#include <gex/prim.hpp>
#include <gex/algorithm/multiOffset.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace pattern
    {

      template<typename PRIMITIVE, typename POINT>
      struct Concentric
      {
        typedef POINT point_type;
        typedef typename point_type::Scalar scalar_type;
        typedef prim::MultiLineString<point_type> output_type;

        Concentric(const PRIMITIVE& _primitive, 
            scalar_type _distance,
            size_t _number = 0) :
          distance_(_distance),
          number_(_number),
          primitive_(_primitive) {}

        template<typename BOUNDS>
        void operator()(const BOUNDS& _bounds, output_type& _output)
        {
          typedef gex::prim::Ring<point_type> ring_type;
          typedef gex::prim::LineString<point_type> linestring_type;
          std::vector<prim::MultiPolygon<gex::prim::Polygon<ring_type>>> _offsets;
          multiOffset(primitive_,algorithm::strategy::DistanceNumber(-distance(),number(),0.5 *-distance()),_offsets);

          for (auto& _offset : _offsets)
            for_each<ring_type>(_offset,[&](const ring_type& _ring)
            {
              _output.emplace_back(gex::convert<linestring_type>(_ring));
            });
        }

        TBD_PROPERTY_REF_RO(scalar_type,distance)
        TBD_PROPERTY_REF_RO(size_t,number)
      private:
        PRIMITIVE const& primitive_;
      };
    }
  }
}

