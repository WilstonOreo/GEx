#pragma once

#include "Model.hpp"

namespace gex
{
    namespace base
    {
      /// Template class representing a range in a min() and max() value
      template<class SCALAR>
      struct Range
      {
        typedef SCALAR scalar_type;

        Range(scalar_type _min = -misc::inf<scalar_type>(),
              scalar_type _max = misc::inf<scalar_type>()) : 
          min_(_min), max_(_max)
        {
          validate();
        }

        void validate()
        {
          if (min_ > max_) std::swap(min_,max_);
        }

        scalar_type length() const
        {
          return max_ - min_;
        }

        scalar_type middle() const
        {
          return ( min_ + max_ ) / 2;
        }

        bool nul() const { return max_ == min_; }
        bool nul(scalar_type _tolerance) const { return max_ - min_ < _tolerance; }

        void operator()(scalar_type _min, scalar_type _max)
        {
          min_=_min;
          max_=_max;
          validate();
        }

        friend bool operator<(const Range& _l, const Range& _r)
        {
          return _l.middle() < _r.middle();
        }

        friend std::ostream& operator<<(std::ostream& _os, const Range& _r)
        {
          _os << '[' << _r.min() << ',' << _r.max() << ']';
          return _os;
        }

        TBD_PROPERTY_REF_MON(scalar_type,min,validate)
        TBD_PROPERTY_REF_MON(scalar_type,max,validate)
      };
    }
  }
