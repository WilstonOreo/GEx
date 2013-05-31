#pragma once

#include "Buffer.hpp"
#include "Point.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<>
      struct Draw<Segment>
      {
        void operator()(const Segment& _segment, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          std::stringstream _parameter;
          auto _p0 = _buffer.transform(_segment[0]);
          auto _p1 = _buffer.transform(_segment[1]);
          _parameter << 
            "x1=\"" << _p0.x() << "\" " << 
            "y1=\"" << _p0.y() << "\" " <<
            "x2=\"" << _p1.x() << "\" " << 
            "y2=\"" << _p1.y() << "\"";
          _buffer += Shape("line",_parameter.str(),_style);

          Draw<Point2>()(_segment[0],"stroke:yellow",_buffer,5);
          Draw<Point2>()(_segment[1],"stroke:white",_buffer,7);
        }
      };
    }
  }
}
