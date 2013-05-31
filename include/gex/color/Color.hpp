#pragma once

#include <array>
#include <boost/mpl/int.hpp>
#include "gex/misc.hpp"
#include "gex/base/Model.hpp"

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
          v_[3] = max();
      }
      
      Color(component_type _r, component_type _g, component_type _b, component_type _a )
      {
        BOOST_STATIC_ASSERT(dimensions() == 4);
        this->operator()(_r,_g,_b,_a);
      }
      
      static const component_type max()
      {
        return max(component_type());
      }
      
      const component_type& r() const
      {
        return v_[0];
      }
      
      void r( const component_type& _value )
      {
        v_[0] = _value;
      }
      
      const component_type& g() const
      {
        return v_[1];
      }
      
      void g( const component_type& _value )
      {
        v_[1] = _value;
      }
      
      const component_type& b() const
      {
        return v_[2];
      }
      
      void b( const component_type& _value )
      {
        v_[2] = _value;
      }
      
      const component_type a() const
      {
        return a(boost::mpl::int_<dimensions()>());
      }
      void a( const component_type& _value )
      {
        BOOST_STATIC_ASSERT(dimensions() >= 4);
        v_[3] = _value;
      }

      const component_type* values() const
      {
        return v_;
      }
      component_type* values()
      {
        return v_;
      }
      operator const component_type*() const
      {
        return v_.data();
      }
      operator component_type*()
      {
        return v_.data();
      }

      void operator += ( const Color& _c )
      {
        auto it = _c.v_.begin();
        for (auto& c : v_)
        {
          c += *it; ++it;
        }
      }
      void operator -= ( const Color& _c )
      {
        auto it = _c.v_.begin();
        for (auto& c : v_)
        {
          c -= *it; ++it;
        }
      }
      void operator *= ( component_type _f )
      {  
        for (auto& c : v_) c *= _f;
      }

      friend Color operator*( const Color& _a, const component_type _f )
      {
        Color v(_a);
        for (auto& c : v.v_) c *= _f;
        return v;
      }
      friend Color operator*( const component_type _f, const Color& _a )
      {
        return _a*_f;
      }
      friend Color operator*( const Color& _a, const Color& _b)
      {
        Color v;
        auto aIt = _a.v_.begin(),
             bIt = _b.v_.begin();
        for (auto& c : v.v_) 
        {
          c = (*aIt) * (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }
      friend Color operator-( const Color& _a, const Color& _b)
      {
        Color v; 
        auto aIt = _a.v_.begin(),
             bIt = _b.v_.begin();
        for (auto& c : v.v_) 
        {
          c = (*aIt) - (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }
      friend Color operator+( const Color& _a, const Color& _b)
      {
        Color v; 
        auto aIt = _a.v_.begin(),
             bIt = _b.v_.begin();
        for (auto& c : v.v_) 
        {
          c = (*aIt) + (*bIt);
          ++aIt; ++bIt;
        }
        return v;
      }

      inline void operator()(const Color& _color)
      {
        v_ = _color.v_;
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
          float _diff = (_lhs.v_[i] - _rhs.v_[i]) / _lhs.max();
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
        return v_[3];
      }
      const component_type a(boost::mpl::int_<3>) const
      {
        return max();
      }

    protected:
      array_type v_;
    };

    template<class SCALAR=DEFAULT_COLOR_TYPE> using Color3 = Color<Model<3,SCALAR>>;
    template<class SCALAR=DEFAULT_COLOR_TYPE> using Color4 = Color<Model<4,SCALAR>>;
  }
}

