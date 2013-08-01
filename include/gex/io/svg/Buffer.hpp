#pragma once


#include "gex/prim.hpp"
#include "Color.hpp"
#include "Draw.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      struct Buffer
      {
        typedef Point2 point_type;
        typedef Vec2 vec_type;
        typedef typename point_type::scalar_type scalar_type;
        typedef Style style_type;
        typedef Color color_type;

        Buffer(int _width = 1024, int _height = 1024) : 
          width_(_width),
          height_(_height),
          scale_(1,1), 
          offset_(0,0)
        {}

        void operator += (const Shape& _s)
        {
          buffer_.push_back(_s.str());
        }

        /// Blackens the image and resizes it if given dimensions are != 0
        void clear(int _width = 0, int _height = 0)
        {
          resize(_width,_height); 
          buffer_.clear();
        }

        void resize(int _width, int _height)
        {
          if (_height && _width) 
          {
            width_ = _width;
            height_ = _height;
          }
        }

        template<typename POS, typename TEXT>
        void text(const POS& _pos, const TEXT& _text, const Style& _style)
        {
          std::stringstream _textStr;
          auto&& _p = transform(_pos);
          _textStr << "<text " << _style << " ";
          _textStr << "x=\"" << _p.x() << "\" y=\"" << _p.y() << "\">" << _text;
          _textStr << "</text>";
          buffer_.push_back(_textStr.str());
        }

        void write(const std::string& _filename) const 
        {
          std::ofstream _os(_filename.c_str());

          _os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
          _os << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
          _os << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
              << "xmlns:xlink=\"http://www.w3.org/1999/xlink\" "  
              << "xmlns:ev=\"http://www.w3.org/2001/xml-events\" "
              << "version=\"1.1\" baseProfile=\"full\" "
              << "width=\"" << width_ << "px\" "  
              << "height=\"" << height_ << "px\"> " << std::endl;

          if (backgroundColor_.a() <= 0)
            _os << background();

          for (const auto& _s : buffer_) _os << _s << std::endl;

          _os << "</svg>" << std::endl;
        }

        template<typename POINT>
        point_type transform(const POINT& _p) const
        {
          return offset_ + scaleVec(_p);
        }

        template<typename VEC>
        vec_type scaleVec(const VEC& _p) const
        {
          return vec_type(_p) * scale_;
        }

        template<typename RECT>
        void fit(const RECT& _rect)
        {
          scale(vec_type(width_ / _rect.size().x(), height_ / _rect.size().y()));
          offset(-_rect.min()*scale());
        }

        TBD_PROPERTY_RO(int,width)
        TBD_PROPERTY_RO(int,height)

        TBD_PROPERTY_REF(color_type,backgroundColor)
        TBD_PROPERTY_REF(vec_type,scale)
        TBD_PROPERTY_REF(vec_type,offset)
      private:
        Shape background() const
        {
          std::stringstream _params, _style;
          _params << "width=\"" << width_ << "\" ";
          _params << "height=\"" << height_ << "\" ";
          _style << "fill:" << backgroundColor_ << ";";
          _style << "stroke-width:0;";

          return Shape("rect",_params.str(),_style.str());
        }

        std::vector<std::string> buffer_;
      };
    }
  }
}

