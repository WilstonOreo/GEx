#pragma once 

#include "Buffer.hpp"

namespace gex 
{
  namespace io
  {
    namespace svg
    {
      template<>
      struct Draw<Bounds2>
      { 
        void operator()(const Bounds2& _bounds, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          std::stringstream ss;
          auto _min = _buffer.transform(_bounds.min());
          auto _size = _buffer.scaleVec(_bounds.size());

          ss << "x=\"" <<  _min.x() << "\" ";
          ss << "y=\"" << _min.y() << "\" ";
          ss << "width=\"" << _size.x() << "\" ";
          ss << "height=\"" << _size.y() << "\"";
          _buffer += Shape("rect",ss.str(),_style);
        }
      };
    }
  }
}

