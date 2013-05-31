#pragma once

#include "LineString.hpp"
#include "gex/base/Point.hpp"
#include "gex/algorithm/boost.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>

namespace gex
{
    namespace prim
    {
      template<class MODEL>
      struct Ring :
          Primitive<MODEL>,
          std::vector<base::Point<MODEL>>
      {
        GEX_PRIMITIVE(MODEL)
        enum Location : unsigned char { INNER, OUTER };
        enum Orientation : unsigned char { CW , CCW };
        typedef std::vector<point_type> ctnr_type;

        /// Default constructor
        Ring(Location _location = OUTER) :
          location_(_location),
          correct_(false),
          length_(0.0),
          area_(0.0)
        {
        }

        Ring(const LineString<MODEL>& _lineString) :
          correct_(false),
          length_(0.0),
          area_(0.0)
        {
          if (_lineString.size() < 2) return;
          
          auto _p = _lineString.front();
          push_back(_p);

          for (auto it = _lineString.begin()+1 ; it != _lineString.end() ; ++it)
          {
            if ((this->back() - *it).sqrLength() > 0.000001) {  push_back(*it); }
          }
          
          location_ = (algorithm::area(*this) > 0) ? OUTER : INNER; 
          update();
        }

        void clear()
        {
          correct_ = false;
          ctnr_type::clear();
        }

        void push_back(point_type _point)
        {
          correct_ = false;
          ctnr_type::push_back(_point);
        }

        Orientation orientation() const
        {
          return algorithm::area(*this) > 0 ? CCW : CW; 
        }

        void update()
        {
          if (correct_) return;

          algorithm::correct(*this);
          algorithm::envelope(*this,bounds_);
          length_ = algorithm::perimeter(*this);
          area_ = algorithm::area(*this);
          
          correct_ = true;
        }
       
        const bounds_type& bounds() const
        {
          return bounds_;
        }

        TBD_PROPERTY(Location,location)
        TBD_PROPERTY_RO(bool,correct)
        TBD_PROPERTY_RO(scalar_type,length)
        TBD_PROPERTY_RO(scalar_type,area)

      private:
        bounds_type bounds_;
      };
    }
  }


