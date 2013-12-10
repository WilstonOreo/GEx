#pragma once

#include <gex/prim.hpp>

namespace gex
{
  namespace strategy
  {
    namespace offset
    {
      struct Default 
      {
        typedef Scalar offset_type;

        Default(const Scalar& _offset) : 
          offset_(_offset) {}

        TBD_PROPERTY_REF(Scalar,offset)
      };

      template<typename PRIMITIVE>
      struct NonUniform
      {};

      template<typename POINT>
      struct NonUniform<prim::Segment<POINT>>
      {
        typedef std::pair<Scalar,Scalar> offset_type;
        explicit NonUniform(const offset_type& _offset) :
          offset_(_offset) 
        {}

        TBD_PROPERTY(offset_type,offset)
      };

      struct MultiDistanceNumber
      {
        MultiDistanceNumber(
            double _distance,
            size_t _number = 0,
            double _offset = 0.5,
            double _simplify = 0.0) :
          distance_(_distance),
          number_(_number),
          offset_(_offset),
          simplify_(_simplify) {}

        TBD_PROPERTY_REF_RO(double,distance)
        TBD_PROPERTY_REF_RO(size_t,number)
        TBD_PROPERTY_REF_RO(double,offset)
        TBD_PROPERTY_REF_RO(double,simplify)
      };

      struct MultiDistances
      {
        typedef std::set<double,std::greater<double>> set_type;

        MultiDistances(
            const set_type& _distances,
            double _simplify = 0.0) :
          distances_(_distances),
          simplify_(_simplify) {}
        
        TBD_PROPERTY_REF_RO(set_type,distances)
        TBD_PROPERTY_REF_RO(double,simplify)
      };

      template<typename POINT>
      struct MultiWithDetails
      {
        typedef prim::LineString<POINT> linestring_type;
        typedef prim::MultiLineString<POINT> multilinestring_type;
        typedef std::vector<std::pair<size_t,size_t>> indices_type;

        MultiWithDetails(double _distance, size_t _number, 
            multilinestring_type& _details,
            indices_type& _indices) :
          distance_(_distance),
          number_(_number),
          details_(_details),
          indices_(_indices) {}

        multilinestring_type& details()
        {
          return details_;
        }
        
        multilinestring_type const& details() const
        {
          return details_;
        }
        
        indices_type& indices()
        {
          return indices_;
        }
        
        indices_type const& indices() const
        {
          return indices_;
        }

        TBD_PROPERTY_REF_RO(double,distance)
        TBD_PROPERTY_REF_RO(size_t,number)
      private:
        multilinestring_type& details_;
        indices_type& indices_;
      };
    }
  }
}

