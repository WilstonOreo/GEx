#pragma once
#include <gex/misc.hpp>
#include "offset.hpp"
#include "medial_axis.hpp"

namespace gex
{
  namespace algorithm
  {
    GEX_FUNCTOR(MultiOffset,multi_offset)

    namespace strategy
    {
      struct DistanceNumber
      {
        DistanceNumber(
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

      struct Distances
      {
        typedef std::set<double,std::greater<double>> set_type;

        Distances(
            const set_type& _distances,
            double _simplify = 0.0) :
          distances_(_distances),
          simplify_(_simplify) {}
        
        TBD_PROPERTY_REF_RO(set_type,distances)
        TBD_PROPERTY_REF_RO(double,simplify)
      };

      template<typename POINT>
      struct DistanceNumberWithDetails
      {
        typedef prim::MultiLineString<POINT> linestring_type;

        DistanceNumberWithDetails(double _distance, size_t _number, linestring_type& _details) :
          distance_(_distance),
          number_(_number),
          details_(_details) {}

        linestring_type& details()
        {
          return details_;
        }

        TBD_PROPERTY_REF_RO(double,distance)
        TBD_PROPERTY_REF_RO(size_t,number)
      private:
        linestring_type& details_;
      };
    }

    namespace functor
    {
      template<typename PRIMITIVE>
      struct MultiOffset<PRIMITIVE,strategy::Distances>
      {
        typedef MultiPolygon output_type;

        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          PRIMITIVE _simplified;
          if (_strategy.simplify() > 0)
            gex::algorithm::simplify(_primitive,_simplified,_strategy.simplify());
          else
            _simplified = _primitive;

          typename OUTPUT::value_type _perimeters, _out, _offsetPolygons;
          for (auto& _offset : _strategy.distances())
          {
            _offsetPolygons.clear();
            offset(_simplified,_offset,_offsetPolygons);
            std::cout << _offset << std::endl;
            if (_offsetPolygons.empty()) break;
            
            _offsetPolygons.update();
            _output.push_back(_offsetPolygons);
          }
        }
      };

      template<typename PRIMITIVE>
      struct MultiOffset<PRIMITIVE,strategy::DistanceNumber>
      {
        typedef MultiPolygon output_type;
        
        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          PRIMITIVE _simplified;
          if (_strategy.simplify() > 0)
            gex::algorithm::simplify(_primitive,_simplified,_strategy.simplify());
          else
            _simplified = _primitive;
          
          typename OUTPUT::value_type _perimeters, _out, _offsetPolygons;

          if (!_strategy.number())
          {
            double _offset = _strategy.offset();
            while (1)
            {
              std::cout << _offset << std::endl;
              _offsetPolygons.clear();
              offset(_simplified,_offset,_offsetPolygons);
              if (_offsetPolygons.empty()) break;

              _offsetPolygons.update();
              _output.push_back(_offsetPolygons);
              _offset += _strategy.distance();
            }
          } else
          {
            strategy::Distances::set_type _set;
            for (int i = 0; i < _strategy.number(); i++)
              _set.insert(i * _strategy.distance() + _strategy.offset());
            MultiOffset<PRIMITIVE,strategy::Distances>()(_primitive,strategy::Distances(_set,_strategy.simplify()),_output);
          }
        }
      };

      template<typename PRIMITIVE, typename POINT>
      struct MultiOffset<PRIMITIVE,strategy::DistanceNumberWithDetails<POINT>>
      {
        typedef MultiPolygon output_type;
        
        template<typename STRATEGY, typename OUTPUT>
        void operator()(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output)
        {
          output_type _offsetPolygons, _lastPolygons;
          for (int i = 0; i < _strategy.number(); i++)
          {
            auto& _width = _strategy.distance();
            float _offset = -_width*(i+0.5);
            offset(_primitive,_offset,_offsetPolygons);
            _lastPolygons = offset(_primitive,-_width*(i-1.0));
            auto&& _polygons = offset(_primitive,-_width*i);

            typedef typename STRATEGY::linestring_type linestring_type;
            linestring_type _remaining;
            auto&& _medial_axis = convert<linestring_type>(medial_axis(!i ? _polygons : _lastPolygons,gex::strategy::medial_axis::Pruning(_width)));
            boost::geometry::correct(_medial_axis);
            boost::geometry::intersection(_medial_axis,_lastPolygons,_remaining);
            _medial_axis.clear();
            boost::geometry::difference(_remaining,_offsetPolygons,_medial_axis);

            for (auto& _lineString : _medial_axis)
              _strategy.details().push_back(_lineString);

            if (_offsetPolygons.empty()) break;
          }
        }
      };
    }
  }

  using algorithm::multi_offset;
}

