#pragma once

#include <array>
#include <boost/mpl/int.hpp>
#include "gex/misc.hpp"
#include "gex/base/Model.hpp"
#include "gex/base/Coords.hpp"
#include <sstream>

namespace gex
{
  namespace color
  {
    typedef float DEFAULT_COLOR_TYPE;

    template<typename T, typename MIN, typename MAX>
    static T clamp(T _t, MIN _min, MAX _max)
    {
      if (_t > _max) return _max;
      if (_t < _min) return _min;
      return _t;
    }

    using base::Model;

    template<typename MODEL>
    struct Color : MODEL
    {
      GEX_MODEL(MODEL)

      typedef scalar_type component_type;
      static constexpr size_t channels() { return dimensions(); }
      typedef std::array<component_type,channels()> array_type;

      Color()
      {
        BOOST_STATIC_ASSERT(dimensions() >= 3 && dimensions() <= 4);
      }

      Color(component_type _r, component_type _g, component_type _b )
      {
        BOOST_STATIC_ASSERT(dimensions() >= 3 && dimensions() <= 4);
        this->operator()(_r,_g,_b);
        if( dimensions() > 3 )
          a_[3] = max();
      }
      
      Color(component_type _r, component_type _g, component_type _b, component_type _a )
      {
        BOOST_STATIC_ASSERT(dimensions() == 4);
        this->operator()(_r,_g,_b,_a);
      }

      template<typename COLOR>
      Color(const COLOR& _color)
      {
        this->operator()(_color.r(),_color.g(),_color.b());
        if (dimensions() == 4)
          a_[3] = _color.dimensions() == 4 ? _color.a() : max();
      }

      static const component_type max()
      {
        return max(component_type());
      }
      
      GEX_SCALAR_REF(r,0)
      GEX_SCALAR_REF(g,1)
      GEX_SCALAR_REF(b,2)

      component_type& a()
      {
        checkIndex<3>();
        return a_[3];
      }

      const component_type a() const
      {
        return a(boost::mpl::int_<dimensions()>());
      }

      void a( const component_type& _value )
      {
        checkIndex<3>();
        a_[3] = _value;
      }

      const component_type* values() const
      {
        return a_;
      }
      component_type* values()
      {
        return a_;
      }
      operator const component_type*() const
      {
        return a_.data();
      }
      operator component_type*()
      {
        return a_.data();
      }

      void operator += ( const Color& _c )
      {
        auto it = _c.a_.begin();
        for (auto& c : a_)
        {
          c += *it; ++it;
        }
      }
      void operator -= ( const Color& _c )
      {
        auto it = _c.a_.begin();
        for (auto& c : a_)
        {
          c -= *it; ++it;
        }
      }
      void operator *= ( component_type _f )
      {  
        for (auto& c : a_) c *= _f;
      }

      friend Color operator*( const Color& _a, const component_type _f )
      {
        Color v(_a);
        for (auto& c : v.a_) c *= _f;
        return v;
      }
      friend Color operator*( const component_type _f, const Color& _a )
      {
        return _a*_f;
      }
      friend Color operator*( const Color& _a, const Color& _b)
      {
        Color v;
        auto aIt = _a.a_.begin(),
             bIt = _b.a_.begin();
        for (auto& c : v.a_) 
        {
          c = (*aIt) * (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }
      friend Color operator-( const Color& _a, const Color& _b)
      {
        Color v; 
        auto aIt = _a.a_.begin(),
             bIt = _b.a_.begin();
        for (auto& c : v.a_) 
        {
          c = (*aIt) - (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }
      friend Color operator+( const Color& _a, const Color& _b)
      {
        Color v; 
        auto aIt = _a.a_.begin(),
             bIt = _b.a_.begin();
        for (auto& c : v.a_) 
        {
          c = (*aIt) + (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }

      inline void operator()(const Color& _color)
      {
        a_ = _color.a_;
      }

      inline void operator()(component_type _r, component_type _g, component_type _b)
      {
        r(_r);
        g(_g);
        b(_b);
      }
      inline void operator()(component_type _r, component_type _g, component_type _b, component_type _a)
      {
        r(_r);
        g(_g);
        b(_b);
        a(_a);
      }

      friend float diff(const Color& _lhs, const Color& _rhs)
      {
        float _sum = 0;

        /// @todo improve this
        for (size_t i = 0; i < _lhs.channels(); i++)
        {
          float _diff = (_lhs.a_[i] - _rhs.a_[i]) / _lhs.max();
          _sum += _diff * _diff;
        }

        return _sum / channels();

      }


      static float max(float)
      {
        return 1.0f;
      }
      static double max(double)
      {
        return 1.0;
      }
      static uint8_t max(uint8_t)
      {
        return 255;
      }

      static uint16_t max(uint16_t)
      {
        return 65535;
      }

      static uint32_t max(uint32_t)
      {
        return 0xFFFFFFFF;
      }

      const component_type a(boost::mpl::int_<4>) const
      {
        return a_[3];
      }
      const component_type a(boost::mpl::int_<3>) const
      {
        return max();
      }

      template<class OSTREAM>
      inline OSTREAM& format( OSTREAM& _s ) const
      {
        bool _f=true;
        for( auto& _c : a_ )
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

      friend std::ostream& operator<<(std::ostream& _os, const Color& _c)
      {
        return _c.format(_os);
      }

      friend std::istream& operator>>(std::istream& _is, Color& _c)
      {
        std::vector<std::string> _tokens;
        tbd::parse(_is,_tokens,"(",")",",",1);
        if (_tokens.size() != _c.channels()) return _is;
        for (size_t i = 0; i < _c.channels(); ++i)
        {
          std::stringstream _iss(_tokens[i]);
          _iss >> _c[i];
        }
        return _is;
      }

    private:
      template<size_t INDEX>
      static void checkIndex()
      {
        static_assert(dimensions() > INDEX,"Channel index must be smaller than number of channels.");
      }
      array_type a_;
    };

    template<class SCALAR=DEFAULT_COLOR_TYPE> using Color3 = Color<Model<3,SCALAR>>;
    template<class SCALAR=DEFAULT_COLOR_TYPE> using Color4 = Color<Model<4,SCALAR>>;
  }
}

