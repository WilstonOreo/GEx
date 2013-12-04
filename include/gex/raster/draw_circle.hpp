#pragma once

#include "for_each.hpp"

#include <cmath>
#include <gex/color/Color.hpp>

namespace gex
{
  namespace raster
  {
    using gex::color::clamp;

    template<typename RADIUS, typename OFFSET, typename IMAGE>
    void draw_circle(const RADIUS& _radius, const OFFSET& _offset, IMAGE& _image)
    {
      size_t _left   = std::max(size_t(_offset.x() - _radius - 1),size_t(0));
      size_t _top    = std::max(size_t(_offset.y() - _radius - 1),size_t(0));
      size_t _right  = std::min(size_t(_offset.x() + _radius + 1),_image.width());
      size_t _bottom = std::min(size_t(_offset.y() + _radius + 1),_image.height());

      typedef typename IMAGE::pixel_type pixel_type;
      for_each(_image,strategy::for_each::PixelInRect(_left,_top,_right,_bottom),[&](pixel_type& _pixel, size_t _x, size_t _y)
      {
        float _dx = _x - _offset.x(), _dy = _y - _offset.y();
        float _d = sqrt(_dx*_dx + _dy*_dy);
        pixel_type _value = clamp((_radius - _d) *255.0,0,255);
        if (_value > 0)
          _pixel = clamp(_pixel + _value,0,255);
      });
    }

  }
}
