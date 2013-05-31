#pragma once 

#include "Buffer.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<>
      struct Draw<Ring>
      { 
        void operator()(const Ring& _ring, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          std::stringstream _points;
          _points << "points=\"";
          for (auto it = _ring.begin(); it != _ring.end() ; ++it)
          {
            auto _p = _buffer.transform(*it);
            _points << _p.x() << "," << _p.y() << (it != --_ring.end() ? " " : "\"");
          }
          _buffer += Shape("polygon",_points.str(),_style);
        }
      };
    }
  }
}
