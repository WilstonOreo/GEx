#pragma once

#include "Generic.hpp"
#include "Ring.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace converter
      {
        template<>
        struct Converter<std::vector<Segment>,std::vector<Ring>>
        {
          
        };

        GEX_CONVERTER(std::vector<Segment>,MultiPolygon)
        {
//          _out = convert<MultiPolygon>(convert<std::vector<Ring>>(_in));
        };
      }
    }
  }
