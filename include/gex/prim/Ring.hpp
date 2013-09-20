#pragma once

#include "LineString.hpp"
#include "gex/base/Bounds.hpp"
#include <boost/geometry/algorithms/perimeter.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include "gex/algorithm/boost.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>

namespace gex
{
  namespace prim
  {
    template<class POINT>
    struct Ring : base::VecContainer<POINT>
    {
      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Vec<model_type> vec_type;
      typedef base::Bounds<model_type> bounds_type;
      typedef base::Range<scalar_type> range_type;

    enum Location :
      unsigned char { INNER, OUTER };
    enum Orientation :
      unsigned char { CW , CCW };
      typedef base::VecContainer<point_type> ctnr_type;

      template<typename BEGIN, typename END>
      Ring(BEGIN _begin, END _end) :
        correct_(false),
        length_(0.0),
        area_(0.0)
      {
        this->reserve(_end - _begin);
        std::for_each(_begin,_end,[&](const point_type& _point)
        {
          push_back(_point);
        });
        location_ = (algorithm::area(*this) > 0) ? OUTER : INNER;
        update();
      }

      /// Default constructor
      Ring(Location _location = OUTER) :
        location_(_location),
        correct_(false),
        length_(0.0),
        area_(0.0)
      {
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
        boost::geometry::envelope(*this,bounds_);
        length_ = boost::geometry::perimeter(*this);
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

    template<typename POINT>
    struct MultiRing : std::vector<Ring<POINT>>
    {
      typedef Ring<POINT> primitive_type;
      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Range<scalar_type> range_type;
      typedef std::vector<point_type> ctnr_type;
      typedef base::Bounds<model_type> bounds_type;
    };
  }
}


