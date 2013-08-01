#pragma once

#include "Primitive.hpp"
#include "gex/base/Point.hpp"
#include <boost/geometry/algorithms/for_each.hpp>

namespace gex
{
    namespace prim
    {
      namespace detail
      {
        template<typename SEGMENT> 
          struct Functor
        {
          Functor(std::vector<SEGMENT>& _segments) :
            segments_(_segments) {}

          template<class T> void operator()( const T& _segment) 
          {
            segments_.push_back(SEGMENT(_segment.first,_segment.second));
          }
          std::vector<SEGMENT>& segments_;
        };
      }


      template<class MODEL>
      struct LineString :
          std::vector<base::Point<MODEL>>,
          Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)
        /// Orientation of ring (CW = clockwise, CCW = counter-clockwise)
        enum Orientation { CW, CCW };

        typedef std::vector<point_type> ctnr_type;

        template<class SEGMENT> 
        void append(const SEGMENT& _segment)
        {
          if (this->empty())
          {
            this->push_back(_segment.p0());
            this->push_back(_segment.p1());
            return;
          }
          if (_segment.p0() != this->back()) this->push_back(_segment.p0());
          if (_segment.p1() != this->back()) this->push_back(_segment.p1());
        }

        void append(const LineString& _lineString)
        {
          if (!this->empty())
            if (_lineString.front() != this->back()) this->push_back(_lineString.front());
          this->reserve(this->size() + _lineString.size());
          this->insert(this->end(),_lineString.begin(),_lineString.end());
        }
      };

      template<class MODEL> 
        struct MultiLineString : 
          std::vector<LineString<MODEL>>, 
          Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)
        typedef LineString<MODEL> linestring_type;
      };
    }
  }
