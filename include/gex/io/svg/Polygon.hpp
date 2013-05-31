#pragma once 

#include "Buffer.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      template<>
      struct Draw<Polygon>
      {
        void operator()(const Polygon& _polygon, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          std::stringstream _points;
          _points << "d=\"";
          _points << pathFromRing(_polygon.boundary(),_buffer);

          for (auto& _hole : _polygon.holes())
            _points << pathFromRing(_hole,_buffer);
          _points << "\"";

          _buffer += Shape("path",_points.str(),_style);
        }

      private:
        std::string pathFromRing(const Ring& _ring, Buffer& _buffer)
        {
          std::stringstream _points;
          for (auto it = _ring.begin(); it != _ring.end() ; ++it)
          {
            auto _p = _buffer.transform(*it);
            _points << (it == _ring.begin() ? "M" : "L") << " ";
            _points << _p.x() << "," << _p.y() << (it != --_ring.end() ? " " : " z ");
          }
          return _points.str();
        }
      };

      template<>
      struct Draw<MultiPolygon>
      {
        void operator()(const MultiPolygon& _multiPolygon, 
                        const Style& _style, 
                        Buffer& _buffer)
        {
          for (auto& _polygon : _multiPolygon)
            Draw<Polygon>()(_polygon,_style,_buffer);
        }
      };
    }
  }
}

