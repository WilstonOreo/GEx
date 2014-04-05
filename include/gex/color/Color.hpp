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
    namespace pixel
    {
      namespace
      {
#define GEX_COLOR_PIXEL_FUNC(func_name,type,return_val)\
        template<> struct func_name<type> { inline type operator()() { return return_val; } };

        template<typename T> struct Max {};
        GEX_COLOR_PIXEL_FUNC(Max,float,1.0f)
        GEX_COLOR_PIXEL_FUNC(Max,double,1.0)
        GEX_COLOR_PIXEL_FUNC(Max,uint8_t,255)
        GEX_COLOR_PIXEL_FUNC(Max,uint16_t,65535)
        GEX_COLOR_PIXEL_FUNC(Max,uint32_t,0xFFFFFFFF)
        
        template<typename T> struct Min {};
        GEX_COLOR_PIXEL_FUNC(Min,float,0.0f)
        GEX_COLOR_PIXEL_FUNC(Min,double,0.0)
        GEX_COLOR_PIXEL_FUNC(Min,uint8_t,0)
        GEX_COLOR_PIXEL_FUNC(Min,uint16_t,0)
        GEX_COLOR_PIXEL_FUNC(Min,uint32_t,0)
      }
      
      template<typename T>
      T min()
      {
        return Min<T>()();
      }
      
      template<typename T>
      T max()
      {
        return Max<T>()();
      }

      template<typename T>
      T def()
      {
        return min<T>();
      }
    }

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
          float _diff = (_lhs.a_[i] - _rhs.a_[i]) / max();
          _sum += _diff * _diff;
        }

        return _sum / channels();

      }

      static component_type max()
      {
        return pixel::max<component_type>();
      }


  friend std::ostream& operator<< (std::ostream& os, const Color& _c)
  {
    os << _c.r() << " " << _c.g() << " ";
    os << _c.b() << " ";
    if (dimensions() == 4) 
      os << _c.a() << " ";
    return os;
  }

  friend std::istream& operator<< (std::istream& is, Color& _c)
  {
    is >> _c.r() >> _c.g() >> _c.b();
    if (dimensions() == 4) 
      is >> _c.a();
    return is;
  }


      const component_type a(boost::mpl::int_<4>) const
      {
        return a_[3];
      }
      const component_type a(boost::mpl::int_<3>) const
      {
        return max();
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

