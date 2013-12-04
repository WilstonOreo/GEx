#pragma once

#include "Image.hpp"
#include "util.hpp"

#include <gex/algorithm/intersects.hpp>

namespace gex
{
  namespace raster
  {
    namespace strategy
    {
      namespace rasterize
      {
        template<typename PRIMITIVE>
        struct Bounds
        {
          typedef typename PRIMITIVE::bounds_type bounds_type;
          Bounds(const bounds_type& _bounds) :
            bounds_(_bounds) {}

          TBD_PROPERTY_REF_RO(bounds_type,bounds)
        };

        struct Default {};
      }
    }

    namespace functor
    {
      template<typename PRIMITIVE> struct Raster {};

      template<typename SCALAR, typename PIXEL>
      struct Marker
      {
        typedef SCALAR scalar_type;
        typedef PIXEL pixel_type;

        Marker() {}
        Marker(
            const scalar_type& _pos,   
            bool _direction) :
          pos_(_pos),
          direction_(_direction) {}

        template<typename ROW, typename SEGMENT>
        Marker(const ROW& _row, const SEGMENT& _segment)
        {
          retrieve(_row,_segment);
        }

        friend bool operator<(const Marker& _lhs, const Marker& _rhs)
        {
          return _lhs.pos() < _rhs.pos();
        }

        template<typename ROW, typename IMAGE>
        scalar_type transformed(const ROW& _row, const IMAGE& _image) const
        {
          scalar_type _tPos = pos_; // Transformed position
          _tPos -= _row.range().min();
          _tPos *= _image.width() / _row.range().length();
          return _tPos;
        }

        template<typename ROW, typename SEGMENT>
        bool retrieve(const ROW& _row, const SEGMENT& _segment)
        {
          auto _bounds = _segment.bounds();
          // Case1, segment does not intersect row
          if ((_bounds.min().y() > _row.pos()) || (_bounds.max().y() < _row.pos()) ||
              (!gex::intersects(_bounds.range(X),_row.range()))) return false; 
          
          direction_ = _segment.p0().y() > _segment.p1().y();
          pos_ = _row.intersect(_segment).x();
          return true;
        }

        TBD_PROPERTY_RO(scalar_type,pos)
        TBD_PROPERTY_RO(bool,direction)
      };


      template<typename SCALAR>
      struct Row
      {
        typedef SCALAR scalar_type;
        typedef base::Range<scalar_type> range_type;

        Row() {}
        Row(const scalar_type& _pos, const range_type& _range, size_t _y) :
          pos_(_pos),
          range_(_range),
          y_(_y)
        {}

        
        template<typename SEGMENT>
        typename SEGMENT::point_type intersect(const SEGMENT& _segment) const
        {
          auto& _p0 = _segment.p0();
          auto& _p1 = _segment.p1();
          typename SEGMENT::point_type _point;
          _point.y() = pos();
          scalar_type _invDet = 1.0 / (_p1.y() - _p0.y());
          scalar_type _t = (_point.y() - _p0.y()) * _invDet; 
          scalar_type _dX =  _p1.x() - _p0.x(); 
          _t = _t < 0.0 ? 0.0 : _t > 1.0 ? 1.0 : _t;
          _point.x() = _p0.x() + _dX * _t;
          return _point;
        }

        TBD_PROPERTY_RO(scalar_type,pos)
        TBD_PROPERTY_REF_RO(range_type,range)
        TBD_PROPERTY_RO(size_t,y)
      };

      template<typename IMAGE, typename BOUNDS, typename F>
      void for_each_row(const IMAGE& _image, const BOUNDS& _bounds, F _f)
      {
        float _pos = _bounds.min().y();
        float _delta = _bounds.size().y() / _image.height();

        typedef typename BOUNDS::point_type point_type;
        typedef typename point_type::Scalar scalar_type;

        for (auto _y = 0; _y < _image.height(); ++_y)
        {
          _f(Row<scalar_type>(_pos,_bounds.range(X),_y));
          _pos += _delta;
        }
      }

      using namespace strategy::rasterize;

      template<typename PRIMITIVE, typename STRATEGY, typename IMAGE>
      struct Rasterize {};
    }

    using functor::Rasterize;

    template<typename PRIMITIVE, typename STRATEGY, typename OUTPUT>
    void rasterize(const PRIMITIVE& _primitive, STRATEGY _strategy, OUTPUT& _output, gex::io::SVG& _svg)
    {
      Rasterize<PRIMITIVE,STRATEGY,OUTPUT>()(_primitive,_strategy,_output,_svg);
    }

    namespace functor
    {
      template<typename POINT, typename P>
      struct Rasterize<prim::Segment<POINT>,Bounds<P>,Image8b>
      {
        template<typename SEGMENT, typename STRATEGY, typename OUTPUT>
        void operator()(const SEGMENT& _seg, STRATEGY _s, OUTPUT& _output)
        {
          typedef POINT point_type;
          typedef typename POINT::Scalar scalar_type;
          typedef typename OUTPUT::pixel_type pixel_type; 
          SEGMENT _tmp = _seg;
          auto& _bounds = _s.bounds();
          
          _tmp.p0() -= _bounds.min();
          _tmp.p1() -= _bounds.min();
          auto&& _size = _bounds.size();
          _tmp.p0().x() *= _output.width() / _size.x();
          _tmp.p0().y() *= _output.height() / _size.y();
          _tmp.p1().x() *= _output.width() / _size.x();
          _tmp.p1().y() *= _output.height() / _size.y();
          
          auto& _x0 = _tmp.p0().x();
          auto& _y0 = _tmp.p0().y();
          auto& _x1 = _tmp.p1().x();
          auto& _y1 = _tmp.p1().y();
          
          bool _steep = std::abs(_tmp.p1().y()- _tmp.p0().y()) > std::abs(_tmp.p1().x()- _tmp.p0().x());
          if (_steep)
          {
            std::swap(_x0,_y0);
            std::swap(_x1,_y1);
          }
          if (_x0 > _x1)
          {
            std::swap(_x0,_x1);
            std::swap(_y0,_y1);
          }
          
          point_type _d = _tmp.p1() - _tmp.p0();

          auto _frac = [](scalar_type _a) { return _a - int(_a); };
          auto _rfrac = [&_frac](scalar_type _a) { return 1.0 - _frac(_a); };

          scalar_type _gradient = _d.y() / _d.x();
          
          // First end point
          point_type _end(round(_x0),_y0);
          _end.y() += _gradient*(_end.x() - _x0);
          scalar_type _xGap = _rfrac(_x0 + 0.5);
          point_type _pxl1(_end.x(),int(_end.y()));
          
          auto _plot = [&_output,&_steep](size_t _x, size_t _y, float _v1, float _v2)
          {
            auto&& _max = pixel::max<pixel_type>();
            if (_y >= _output.height()-1) return;
            if (_x >= _output.width()-1) return;

            auto& _p0 = _steep ? _output(_y,_x) : _output(_x,_y);
            auto& _p1 = _steep ? _output(_y+1,_x) : _output(_x,_y+1);
            _p0 = clamp(_p0 + _v1 * _max,0,_max);
            _p1 = clamp(_p1 + _v2 * _max,0,_max);
          };

          _plot(_pxl1.x(),_pxl1.y(),_rfrac(_end.y())*_xGap,_frac(_end.y())*_xGap);

          scalar_type _interY = _end.y() + _gradient;

          // Second end point
          _end = point_type(round(_x1),_y1);
          _end.y() += _gradient*(_end.x() - _x1);
          _xGap = _rfrac(_x1 + 0.5);
          point_type _pxl2(_end.x(),int(_end.y()));
//          _plot(_pxl2.x(),_pxl2.y(),_rfrac(_end.y())*_xGap,_frac(_end.y())*_xGap);
          
          for (float _x = _pxl1.x()+1; _x <= _pxl2.x(); ++_x)
          { 
            _plot(_x,int(_interY),_rfrac(_interY),_frac(_interY));
            _interY += _gradient;
          }
        }
      };

      template<typename POINT, typename P>
      struct Rasterize<prim::Ring<POINT>,Bounds<P>,Image8b>
      {
        typedef Image8b output_type;

        template<typename T, typename STRATEGY, typename OUTPUT>
        void operator()(const T& _t, STRATEGY _s, OUTPUT& _output, gex::io::SVG& _svg)
        {
          typedef typename T::scalar_type scalar_type;
          typedef typename OUTPUT::pixel_type pixel_type; 
          typedef Marker<scalar_type,pixel_type> marker_type;

          typedef typename STRATEGY::bounds_type bounds_type;
          typedef prim::Segment<POINT> segment_type;

          auto& _bounds = _s.bounds();

          for_each_row(_output,_bounds,[&](const Row<scalar_type>& _row)
          {
            std::vector<marker_type> _markers;
            gex::for_each<segment_type>(_t,[&](const segment_type& _segment)
            {
              marker_type _marker;
              if (_marker.retrieve(_row,_segment))
                _markers.push_back(_marker);
            });

            if (_markers.empty()) return;
            std::sort(_markers.begin(),_markers.end());
            size_t _offset = _output.width() * _row.y();
            auto _fill = [&_output,&_offset](size_t _start, size_t _end)
            {
              for (size_t i = _start + _offset; i < _end + _offset; ++i)
              {
                _output(i) = pixel::max<pixel_type>();
              }
            };
            if (_markers.front().direction())
            {
              auto _t = _markers.front().transformed(_row,_output);
              _fill(0,_t);
            }
            for (auto& _marker : _markers)
            {
              std::string _color = _marker.direction() ? "blue" : "red";
              _svg.draw(gex::Point2(_marker.pos(),_row.pos()),
                  "stroke:"+_color+";fill:none;stroke-width:0.4");
            }
            
            if (_markers.empty()) return;

            for (int i = 0; i < _markers.size() - 1; ++i)
            {
              auto& _prev = _markers[i];
              auto& _next = _markers[i+1];
              if (!_prev.direction() || _next.direction())
              {
                auto _tPrev = _prev.transformed(_row,_output);
                auto _tNext = _next.transformed(_row,_output);
                _fill(round(_tPrev+0.5),round(_tNext+0.5));
              }
            }
            _svg.draw(gex::Segment(
                gex::Point2(_bounds.min().x(),_row.pos()),
                gex::Point2(_bounds.max().x(),_row.pos())),
              "stroke:silver;fill:none;stroke-width:0.1");
          });
          // Antialiasing
          gex::for_each<segment_type>(_t,[&](const gex::Segment& _segment)
          {
             Rasterize<segment_type,STRATEGY,OUTPUT>()(_segment,_s,_output);
          });
        }
      };

      template<typename POINT>
      struct Rasterize<prim::Ring<POINT>,Default,Image8b>
      {
        typedef Image8b output_type;

        template<typename T, typename STRATEGY, typename OUTPUT>
        void operator()(const T& _t, STRATEGY, OUTPUT& _output, gex::io::SVG& _svg)
        {
          auto& _bounds = _t.bounds();
          auto&& _center = _bounds.center();
          auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
          typename T::point_type _m(_max,_max);
          typename std::decay<decltype(_bounds)>::type _maxBounds(_center-_m,_center+_m);
          rasterize(_t,Bounds<T>(_maxBounds),_output,_svg);
        }
      };

      template<typename RING, typename STRATEGY, typename OUTPUT>  
      struct Rasterize<prim::Polygon<RING>,STRATEGY,OUTPUT> : 
        Rasterize<RING,STRATEGY,OUTPUT> 
      {
        typedef OUTPUT output_type;
      };
      
      template<typename POLYGON, typename STRATEGY, typename OUTPUT> 
      struct Rasterize<prim::MultiPolygon<POLYGON>,STRATEGY,OUTPUT> : 
        Rasterize<POLYGON,STRATEGY,OUTPUT> 
      {
        typedef OUTPUT output_type;
      };
    }
  }
}
