#pragma once

#include <cstddef>
#include <tbd/add_const_if.h>
#include "util.hpp"

namespace gex
{
  namespace raster
  {
    namespace strategy
    {
      namespace for_each
      {
        struct Pixel {};
        struct PixelInRect
        {
          PixelInRect(size_t _left, size_t _top, size_t _right, size_t _bottom) :
            rect_(Point(_left,_top),Point(_right,_bottom)) 
          {
          }
          PixelInRect(const Rect& _rect) :
            rect_(_rect) {}

          TBD_PROPERTY_REF(Rect,rect)
        };

        struct Neighbors {};
        struct Diff {};
      }
    }

    namespace functor
    {
      using namespace strategy::for_each;

      template<typename IMAGE, typename STRATEGY, bool IS_CONST>
      struct ForEach {};

      template<typename IMAGE, bool IS_CONST>
      struct ForEach<IMAGE,Pixel,IS_CONST>
      {
        typedef typename tbd::AddConstIf<IMAGE,IS_CONST>::ref_type image_ref_type;

        template<typename S, typename F>
        void operator()(image_ref_type _image, S, F f)
        {
          for (size_t _y = 0; _y != _image.height(); ++_y)
          {
            for (size_t _x = 0; _x != _image.width(); ++_x)
            {
              f(_image.pixel(_x,_y),_x,_y);
            }
          }
        }
      };

      template<typename IMAGE, bool IS_CONST>
      struct ForEach<IMAGE,PixelInRect,IS_CONST>
      {
        typedef typename tbd::AddConstIf<IMAGE,IS_CONST>::ref_type image_ref_type;

        template<typename S, typename F>
        void operator()(image_ref_type _image, S _s, F f)
        {
          for (size_t _y = _s.rect().min().y(); _y != _s.rect().max().y(); ++_y)
          {
            for (size_t _x = _s.rect().min().x(); _x != _s.rect().max().x(); ++_x)
            {
              f(_image.pixel(_x,_y),_x,_y);
            }
          }
        }
      };

      template<typename IMAGE, bool IS_CONST>
      struct ForEach<IMAGE,Neighbors,IS_CONST>
      {
        typedef typename tbd::AddConstIf<IMAGE,true>::ref_type image_ref_type;

        template<typename S, typename F>
        void operator()(image_ref_type _image, S, F f)
        {
          if (_image.height() < 2 || _image.width() < 2) return;

          for (size_t _y = 0; _y != _image.height()-1; ++_y)
          {
            for (size_t _x = 0; _x != _image.width()-1; ++_x)
            {
              auto& _topLeft = _image(_x,_y);
              auto& _topRight = _image(_x+1,_y);
              auto& _bottomLeft = _image(_x,_y+1);
              auto& _bottomRight = _image(_x+1,_y+1);
              /// Top Left, Bottom Left 
              f(_topLeft,_topRight,_bottomLeft,_bottomRight,_x,_y);
            }
          }
        }
      };

      
      template<typename IMAGE, bool IS_CONST>
      struct ForEach<IMAGE,Diff,IS_CONST>
      {
        typedef typename tbd::AddConstIf<IMAGE,true>::ref_type image_ref_type;

        template<typename S, typename F>
        void operator()(image_ref_type _image, S, F f)
        {
          typedef typename IMAGE::pixel_type pixel_type;

          ForEach<IMAGE,Neighbors,true>()(_image,Neighbors(),
              [&f](const pixel_type& _p00, const pixel_type& _p01,  
                 const pixel_type& _p10, const pixel_type& _p11,
                 size_t _x, size_t _y)
          {
            /// Pixel, horizontal diff, vertical diff, diagonal diff
            f(_p00,_p00-_p01,_p00-_p10,_p00-_p11,_x,_y);
          });
        }
      };
    }

    using functor::ForEach;

    template<typename IMAGE, typename STRATEGY, typename F>
    void for_each(const IMAGE& _image, STRATEGY _s, F f)
    {
      ForEach<IMAGE,STRATEGY,true>()(_image,_s,f);
    }

    template<typename IMAGE, typename STRATEGY, typename F>
    void for_each(IMAGE& _image, STRATEGY _s, F f)
    {
      ForEach<IMAGE,STRATEGY,false>()(_image,_s,f);
    }

    template<typename IMAGE, typename F>
    void for_each(const IMAGE& _image, F f)
    {
      typedef strategy::for_each::Pixel strategy;
      for_each(_image,strategy(),f);
    }

    template<typename IMAGE, typename F>
    void for_each(IMAGE& _image, F f)
    {
      typedef strategy::for_each::Pixel strategy;
      for_each(_image,strategy(),f);
    }
  }
}
