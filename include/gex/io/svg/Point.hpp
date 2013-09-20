#pragma once

#include "Buffer.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<>
      struct Draw<Point2>
      {  
        inline void operator()(const Point2& _point, 
                        const Style& _style, 
                        Buffer& _buffer,
                        float _radius = 5)
        {
          auto _p = _buffer.transform(_point);
          std::stringstream ss;
          ss << "cx=\"" << _p.x() << "\" ";
          ss << "cy=\"" << _p.y() << "\" ";
          ss << "r=\"" << _radius << "\"";

          _buffer += Shape("circle fill=\"none\"",ss.str(),_style);
        }
      };

    template<typename POINT>
    struct Draw<prim::MultiPoint<POINT>> : 
      Draw<std::vector<POINT>>
    {};
    }
  }
}
