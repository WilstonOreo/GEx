#pragma once 

#include "Draw.hpp"
#include "gex/prim/LineString.hpp"
#include <gex/algorithm/for_each.hpp>

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<typename POINT>
      struct Draw<prim::LineString<POINT>>
      {
        typedef prim::LineString<POINT> linestring_type;
        
        void operator()(const linestring_type& _lineString, 
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


      template<typename POINT>
      struct Draw<prim::MultiLineString<POINT>>
      {
        typedef prim::MultiLineString<POINT> multilinestring_type;
        
        void operator()(const multilinestring_type& _multilineString,
                        const Buffer::style_type& _style,
                        Buffer& _buffer)
        {
          typedef prim::LineString<POINT> linestring_type;
          for_each<linestring_type>(_multilineString,[&](const linestring_type& _lineString)
          {
            Draw<linestring_type>(_lineString,_style,_buffer);
          });
        }
      };
    }
  }
}

