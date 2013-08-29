#pragma once 

#include "Buffer.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename POINT>
      struct Draw<prim::Ring<POINT>>
      { 
        void operator()(const prim::Ring<POINT>& _ring, 
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

      template<typename POINT>
      struct Draw<prim::MultiRing<POINT>>
      {
        void operator()(const prim::MultiRing<POINT>& _multiRing,
                        const Style& _style, 
                        Buffer& _buffer)
        {
          for (auto& _ring : _multiRing) 
            Draw<typename std::decay<decltype(_ring)>::type>()(_ring,_style,_buffer);
        }
      };
    }
  }
}
