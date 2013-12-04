#pragma once

#include <fstream>
#include <gex/base.hpp>
#include "util.hpp"

namespace gex
{
  namespace raster
  {
    template<typename PIXEL>
    struct Image
    {
      typedef PIXEL pixel_type;
      typedef std::vector<pixel_type> data_type;

      Image() {}
      Image(size_t _width, size_t _height)
      {
        resize(_width,_height);
      }

      void put(size_t _x, size_t _y, const pixel_type& _pixel)
      {
        put(_y*width()+_x,_pixel);
      }

      pixel_type& operator()(size_t _x, size_t _y)
      {
        return pixel(_x,_y);
      }
      
      pixel_type const& operator()(size_t _x, size_t _y) const
      {
        return pixel(_x,_y);
      }
      
      pixel_type& operator()(size_t _offset)
      {
        return pixel(_offset);
      }
      
      pixel_type const& operator()(size_t _offset) const
      {
        return pixel(_offset);
      }

      void put(size_t _offset, const PIXEL& _pixel)
      {
        data_[_offset] = _pixel;
      }

      pixel_type& pixel(size_t _x, size_t _y)
      {
        return pixel(_y*width()+_x);
      }

      pixel_type const& pixel(size_t _x, size_t _y) const
      {
        return pixel(_y*width()+_x);
      }

      pixel_type& pixel(size_t _offset)
      {
        return data_[_offset];
      }

      pixel_type const& pixel(size_t _offset) const
      {
        return data_[_offset];
      }

      void clear(const pixel_type& _pixel)
      {
for (auto& _p: data_)
          _p = _pixel;
      }

      void size() const
      {
        return data_.size();
      }

      void clear()
      {
        data_ = data_type(data_.size(),pixel::def<pixel_type>());
      }

      void resize(size_t _width, size_t _height)
      {
        width_ = _width;
        height_ = _height;
        data_.resize(width_*height_);
      }

      template<typename FILENAME>
      void write(const FILENAME& _file) const
      {
        std::ofstream _f(_file,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);

        _f << "P5\n" << width() << " " << height() << "\n255\n";
        _f.write(reinterpret_cast<const char*>(data_.data()),data_.size()*sizeof(pixel_type));

      }

      TBD_PROPERTY_RO(size_t,width)
      TBD_PROPERTY_RO(size_t,height)
      TBD_PROPERTY_REF_RO(data_type,data)
    };

    typedef Image<float> Image32b;
    typedef Image<unsigned char> Image8b;
    typedef Image<bool> Image1b;
  }
}
/*
namespace gex
{
  namespace raster
  {

    template<typename T, typename POS>
    T mix(const T& a, const T& b, const POS& _pos)
    {
      return (_b - _a) * _pos + _a;
    }


    template<typename IMAGE>
    IMAGE contourImage(const IMAGE& _image)
    {
      typedef typename IMAGE::pixel_type pixel_type;

      IMAGE _contourImage(_image.width(),_image.height());
      for_pixel_in_bounds(_image,0,0,_image.width()-1,_image.height()-1,
                          [&](const pixel_type& _pixel, size_t _x, size_t _y)
      {
        auto& _p00 = _pixel;
        auto& _p01 = _image.pixel(_x+1,_y);
        auto& _p10 = _image.pixel(_x,_y+1);
        auto& _p11 = _image.pixel(_x+1,_y+1);
        int _sum = _p00 + _p01 + _p10 + _p11;
        auto& _cPixel = _contourImage.pixel(_x,_y);
        _cPixel = (_sum != 0) && (_sum != 4);
      });
    }

    typedef prim::Point<2,int> Point2i;

    /// TODO use rtree here
    template<typename POINT, typename IMAGE>
    std::map<float,POINT> nearestPoints(const POINT& _p, const IMAGE& _image)
    {
    }

    template<typename IN, typename OUT>
    void medial_axis_points(const IN& _in, OUT& _out)
    {
      typedef typename IN::pixel_type pixel_type;

      auto&& _contourImage = contourImage(_in);

      for_each_pixel(_in,[](const pixel_type& _pixel, size_t _x, size_t _y)
      {
        if (!_pixel || _contourImage.pixel(_x,_y)) return;
        auto&& _points = closestPoints(_x,_y,_contourImage);

        if (points_are_on_axis(_points,_dist))
        {
          _out.put(_x,_y,_dist);
        }
      });
    }

    template<typename AXIS_IMAGE, typename S, typename OUT>
    void scale_axis_transform(const AXIS_IMAGE& _in, S s, OUT& _out)
    {
      typedef typename IN::pixel_type pixel_type;
      auto&& _contourImage = contourImage(_in);
      for_each_pixel(_in,[&](const pixel_type& _pixel, size_t _x, size_t _y)
      {
        if (_pixel < 0) return;
        draw_circle(_pixel*_s,Point2(_x,_y),_out);
      });
    }
  }
*/
