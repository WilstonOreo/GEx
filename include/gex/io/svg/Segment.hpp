#pragma once

#include "Buffer.hpp"
#include "Point.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename POINT>
      struct Draw<prim::Segment<POINT>>
      {
        typedef prim::Segment<POINT> segment_type;

        void operator()(const segment_type& _segment, 
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
          Draw<POINT>()(_segment[0],"stroke:yellow",_buffer,5);
          Draw<POINT>()(_segment[1],"stroke:white",_buffer,7);
        }
      };

      template<typename POINT>
      struct Draw<prim::MultiSegment<POINT>>
      {
        void operator()(const prim::MultiSegment<POINT>& _segments,
                        const Style& _style, 
                        Buffer& _buffer)
        {
          for (auto& _segment : _segments)
            Draw<typename std::decay<decltype(_segment)>::type>()(_segment,_style,_buffer);
        }
      };
    }
  }
}
