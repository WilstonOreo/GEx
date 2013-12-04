#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <tbd/property.h>
#include <tbd/parse_utils.h>
#include <limits>
#include <iostream>

#include "misc/assert.hpp"

#define GEX_FUNCTOR(functor_name,function_name)\
    namespace functor\
    {\
      template<typename IN, typename STRATEGY>\
      struct functor_name {};\
    }\
    using functor::functor_name;\
    template<typename IN, typename STRATEGY, typename OUT>\
    void function_name(const IN& _in, STRATEGY _s, OUT& _out)\
    {\
      functor_name<IN,STRATEGY>()(_in,_s,_out);\
    }\
    template<typename IN, typename STRATEGY>\
    typename functor_name<IN,STRATEGY>::output_type function_name(const IN& _in, STRATEGY _s)\
    {\
      typename functor_name<IN,STRATEGY>::output_type _out;\
      function_name(_in,_s,_out);\
      return _out;\
    }


namespace gex
{
  namespace misc
  {
    namespace defaults
    {
      typedef float ScalarType;
    }

    template<class T = defaults::ScalarType> T deg2rad( T _value )
    {
      return _value * (T)(M_PI / 180.0);
    }

    template<class T = defaults::ScalarType> T rad2deg( T _value )
    {
      return _value * (T)(180.0 / M_PI);
    }

    template<typename T = defaults::ScalarType>
    static const T inf()
    {
      return std::numeric_limits<T>::max();
    }

    template<typename T = defaults::ScalarType>
    static const T rnd()
    {
      return T(rand())/T(RAND_MAX);
    }


    template <typename T> int sgn(T val) 
    {
      return (T(0) < val) - (val < T(0));
    }  
  }

  using misc::deg2rad;
  using misc::rad2deg;
  using misc::rnd;
  using misc::inf;
}


