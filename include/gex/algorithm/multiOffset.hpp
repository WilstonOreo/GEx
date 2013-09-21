#pragma once
#include "offset.hpp"

namespace gex
{
  namespace algorithm
  {
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
    }

    namespace functor
    {
      template<typename PRIMITIVE, typename STRATEGY>
      struct MultiOffset {};
      
      template<typename PRIMITIVE>
      struct MultiOffset<PRIMITIVE,strategy::Distances>
      {
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

    }

    using functor::MultiOffset;

    template<typename IN, typename STRATEGY, typename OUT>
    void multiOffset(const IN& _in, STRATEGY _strategy, OUT& _out)
    {
      MultiOffset<IN,STRATEGY>()(_in,_strategy,_out);
    }
  }

  using algorithm::multiOffset;
}

