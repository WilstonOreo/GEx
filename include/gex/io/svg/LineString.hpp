#pragma once 

#include "Draw.hpp"
#include "gex/prim/LineString.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename SCALAR>
      struct Draw<prim::LineString<SCALAR>>
      {
        typedef prim::LineString<SCALAR> LineString;
        
        void operator()(const LineString& _lineString, 
                        const Buffer::style_type& _style, 
                        Buffer& _buffer)
        {
          std::stringstream _points;
          _points << "points=\"";
          for (auto it = _lineString.begin(); 
               it != _lineString.end() ; ++it)
          {
            auto _p = _buffer.transform(*it);
            _points << _p.x() << "," << _p.y() << (it != --_lineString.end() ? " " : "\"");
          }
          _buffer += Shape("polyline",_points.str(),_style);
        }
      };
    }
  }
}

