#pragma once

#include <sstream>
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

      Range() :
        min_(misc::inf<scalar_type>()),
        max_(-misc::inf<scalar_type>())
      {
      }

      Range(scalar_type _min, scalar_type _max) :
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

      bool nul() const
      {
        return max_ == min_;
      }
      bool nul(scalar_type _tolerance) const
      {
        return max_ - min_ < _tolerance;
      }

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

      void operator+=(scalar_type _r)
      {
        min_ += _r;
        max_ += _r;
      }

      void operator-=(scalar_type _r)
      {
        min_ -= _r;
        max_ -= _r;
      }

      void extend(const Range& _range)
      {
        min_ = std::min(min_,_range.min());
        max_ = std::max(max_,_range.max());
      }

      void extend(const scalar_type& _s)
      {
        min_ = std::min(min_,_s);
        max_ = std::max(max_,_s);
      }

      friend Range operator+(const Range& _l, scalar_type _r)
      {
        Range _range = _l;
        _range += _r;
        return _range;
      }

      friend Range operator-(const Range& _l, scalar_type _r)
      {
        Range _range = _l;
        _range -= _r;
        return _range;
      }

      friend bool operator==(const Range& _lhs, const Range& _rhs)
      {
        return (_lhs.min() == _rhs.min()) &&
               (_rhs.min() == _lhs.min());
      }


      friend std::ostream& operator<< (std::ostream& os, const Range& _range)
      {
        os << _range.min() << " " << _range.max() << " ";
        return os;
      }

      friend std::istream& operator<< (std::istream& is, Range& _range)
      {
        is >> _range.min() >> _range.max();
        return is;
      }

      TBD_PROPERTY_REF_MON(scalar_type,min,validate)
      TBD_PROPERTY_REF_MON(scalar_type,max,validate)
    };
  }
}
