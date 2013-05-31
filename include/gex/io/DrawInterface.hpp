#pragma once

#include "tbd/property.h"
#include <vector>

namespace gex
{
  namespace io
  {
    template<template<class> class DRAW_FUNCTOR, typename BUFFER>
    struct DrawInterface
    {
      DrawInterface(int _width, int _height)
      {
        clear(_width,_height);
      }

      typedef BUFFER buffer_type;
      typedef typename buffer_type::style_type style_type;
      typedef typename buffer_type::scalar_type scalar_type;
      typedef typename buffer_type::vec_type vec_type;
      typedef typename buffer_type::point_type point_type;

      template<typename PRIMITIVE>
      void draw(const PRIMITIVE& _p,
                const style_type& _style)
      {
        DRAW_FUNCTOR<PRIMITIVE>()(_p,_style,buffer_);
      }

      template<typename PRIMITIVE>
      void draw(const std::vector<PRIMITIVE>& _v,
                const style_type& _style)
      {
        for (const auto& _p : _v) 
          DRAW_FUNCTOR<PRIMITIVE>()(_p,_style,buffer_);
      }

      void clear(int _width = 0, int _height = 0)
      {
        buffer_.clear(_width,_height);
      }

      TBD_PROPERTY_REF(buffer_type,buffer)
    };
  }
}
