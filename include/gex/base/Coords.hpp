#ifndef _COORD_HPP
#define _COORD_HPP

#include "gex/misc.hpp"
#include "Model.hpp"
#include "Axis.hpp"

#include <sstream>
#include <array>

namespace gex
{
  namespace base
  {
/// Compiler macro for iterating over each dimension
#define GEX_FOREACH_DIM(i) for (size_t i = 0; i < dimensions(); i++)
#define GEX_FOREACH_DIM_(t,i) for (size_t i = 0; i < t.dimensions(); i++)

    /** @brief Base class of Point and Vec which basically hold a number of coordinates
     * @tparam MODEL Model type
     */
    template<class MODEL>
    struct Coords : MODEL
    {
      GEX_MODEL(MODEL)
      typedef scalar_type value_type;
      typedef std::array<scalar_type,dimensions()> array_type;

      /// Base constructor, all values are initially set to zero
      Coords()
      {
        for (auto& _a : a_) _a = 0;
      }
      Coords( Coords& _coords )
      {
        a_ = _coords.a_;
      }
      Coords( const Coords& _coords )
      {
        a_ = _coords.a_;
      }
      Coords( scalar_type _x, scalar_type _y)
      {
        this->operator()(_x,_y);
      }
      Coords( scalar_type _x, scalar_type _y, scalar_type _z)
      {
        this->operator()(_x,_y,_z);
      }
      Coords( scalar_type _x, scalar_type _y, scalar_type _z, scalar_type _w)
      {
        this->operator()(_x,_y,_z,_w);
      }

      inline void operator()(const Coords _coords)
      {
        a_ = _coords.a_;
      }
      inline void operator()(scalar_type _x, scalar_type _y)
      {
        x(_x);
        y(_y);
      }
      inline void operator()(scalar_type _x, scalar_type _y, scalar_type _z)
      {
        x(_x);
        y(_y);
        z(_z);
      }
      inline void operator()(scalar_type _x, scalar_type _y, scalar_type _z, scalar_type _w)
      {
        x(_x);
        y(_y);
        z(_z);
        w(_w);
      }

      /* @brief Return pointer */
      scalar_type* p()
      {
        return a_.data();
      }
      /* @brief Return const pointer */
      const scalar_type* p() const
      {
        return a_.data();
      }

      const array_type& data() const
      {
        return a_;
      }
      array_type& data()
      {
        return a_;
      }


#define GEX_SCALAR_REF(name,index) \
      inline scalar_type const& name() const\
      {\
        checkIndex<index>(); return a_[index];\
      }\
      inline scalar_type& name()\
      {\
        checkIndex<index>(); return a_[index];\
      }\
      inline void name(const scalar_type& _##name)\
      {\
        checkIndex<index>(); a_[index] = _##name;\
      }

      GEX_SCALAR_REF(x,0)
      GEX_SCALAR_REF(y,1)
      GEX_SCALAR_REF(z,2)
      GEX_SCALAR_REF(w,3)

      /// Methods to access coordinate value in a certain dimension
      scalar_type& operator[] (size_t i)
      {
        return a_[i];
      }
      const scalar_type& operator[] (size_t i) const
      {
        return a_[i];
      }

      void operator += ( const Coords& _c )
      {
        GEX_FOREACH_DIM(i) a_[i] += _c[i];
      }

      void operator -= ( const Coords& _c )
      {
        GEX_FOREACH_DIM(i) a_[i] -= _c[i];
      }

      /// size/dimension of the vector
      static constexpr size_t size()
      {
        return dimensions();
      }

      static const size_t size_ = dimensions();

      Coords vectorize(const scalar_type& _s)
      {
        for (auto& _a : a_)
          _a = _s;
        return *this;
      }

      operator std::string() const
      {
        std::stringstream ss;
        format(ss);
        return ss.str();
      }

      template<class OSTREAM>
      inline OSTREAM& format( OSTREAM& _s ) const
      {
        bool _f=true;
        for( auto& _c : data() )
        {
          if( _f )
          {
            _s << "(" << _c;
            _f = false;
          }
          else
            _s << "," << _c;
        }
        _s << ")";
        return _s;
      }

      friend std::ostream& operator<<(std::ostream& _os, const Coords& _c)
      {
        return _c.format(_os);
      }

      friend std::istream& operator>>(std::istream& _is, Coords& _c)
      {
        std::vector<std::string> _tokens;
        tbd::parse(_is,_tokens,"(",")",",",1);
        if (_tokens.size() != _c.size()) return _is;
        GEX_FOREACH_DIM(i) 
        {
          std::stringstream _iss(_tokens[i]);
          _iss >> _c[i];
        }
        return _is;
      }


      friend bool operator == ( const Coords& _a, const Coords& _b)
      {
        GEX_FOREACH_DIM(i) if (_a[i] != _b[i]) return false;
        return true;
      }

      friend bool operator != ( const Coords& _a, const Coords& _b)
      {
        return !(_a == _b);
      }

    protected:
      template<size_t INDEX>
      static void checkIndex()
      {
        static_assert(dimensions() > INDEX,"Coordinate index must be smaller than number of dimensions.");
      }
      /// Array to store coordinate values
      array_type a_;
    };

#define COORDS(C) C.x(),C.y(),C.z()
  }
}


template<typename OSTREAM, typename MODEL>
inline OSTREAM& operator<<( OSTREAM& _s, const gex::base::Coords<MODEL>& _p )
{
  _p.format(_s);
  return _s;
}

#endif /* _COORD_HPP */

