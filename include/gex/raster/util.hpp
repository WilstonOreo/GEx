#pragma once

#include <gex/base.hpp>
#include <gex/color.hpp>

namespace gex
{
  namespace raster
  {
    namespace pixel
    {
      using namespace gex::color::pixel;
    }

    typedef base::Model<2,int> Model;
    typedef base::Bounds<Model> Rect;
    typedef base::Point<Model> Point;
  }
}
