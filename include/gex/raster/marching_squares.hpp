#pragma once

#include "for_each.hpp"
#include <gex/misc.hpp>
#include <gex/prim.hpp>
#include <gex/io/SVG.hpp>
#include <gex/algorithm/join.hpp>

namespace gex
{
  namespace raster
  {
    namespace strategy
    {
      namespace marching_squares
      {
        struct Segments {};
        struct Polygons {};  
      }
    }
    
    GEX_FUNCTOR(MarchingSquares,marching_squares)
    
    namespace functor
    {
      using namespace strategy::marching_squares;
      
      template<typename IMAGE>
      struct MarchingSquares<IMAGE,Segments>
      {
        typedef gex::MultiSegment output_type;

        template<typename STRATEGY>
        void operator()(const IMAGE& _image, STRATEGY, output_type& _out, gex::io::SVG& _svg)
        {
          typedef typename IMAGE::pixel_type pixel_type;

          for_each(_image,strategy::for_each::Neighbors(),
              [&_out,&_svg](const pixel_type& _p00, const pixel_type& _p01,  
                 const pixel_type& _p10, const pixel_type& _p11,
                 size_t _x, size_t _y)
          {
            int tl = _p00 > 0;
            int tr = _p01 > 0;
            int bl = _p10 > 0;
            int br = _p11 > 0;
            int _config = bl | (br << 1) | (tr << 2) | (tl << 3);
            if (_config == 0 || _config == 15) return;

            typedef typename output_type::point_type point_type;
            auto _max = pixel::max<pixel_type>();

            auto _insert = [&_out,&_config,&_svg](point_type _p0, point_type _p1)
            {
              _out.emplace_back(_p0,_p1);
              
              std::string _color;
              switch (_config)
              {
                case 1: _color = "silver"; break;
                case 2: _color = "gray"; break;
                case 3: _color = "red"; break;
                case 4: _color = "purple"; break;
                case 5: _color = "fuchsia"; break;
                case 6: _color = "blue"; break;
                case 7: _color = "navy"; break;
                case 8: _color = "green"; break;
                case 9: _color = "lime"; break;
                case 10: _color = "teal"; break;
                case 11: _color = "aqua"; break;
                case 12: _color = "olive"; break;
                case 13: _color = "yellow"; break;
                case 14: _color = "orange"; break;
              }
              _svg.draw(_out.back(),"stroke:"+_color+";fill:none");
            };

            float _dUp = float(_p00 - _p01) / _max;
            float _dDown = float(_p10 - _p11) / _max;
            float _dLeft = float(_p00 - _p10) / _max;
            float _dRight = float(_p01 - _p11) / _max;

            std::cout << _dUp << "\t" << _dDown << "\t" << _dLeft << "\t" << _dRight << std::endl;

            point_type _up(_x + _dUp,_y);
            point_type _down(_x + _dDown,_y+1);
            point_type _left(_x,_y + _dLeft);
            point_type _right(_x+1,_y + _dRight);
            point_type _xShift(1.0,0.0);
            point_type _yShift(0.0,1.0);

            switch (_config)
            {
              case 1: _insert(_left + _yShift,_down); break;
              case 11: _insert(_up,_right + _yShift); break;
               
              case 4: _insert(_right,_up+ _xShift); break;
              case 14: _insert(_down + _xShift,_left); break;
              
              case 8: _insert(_up,_left); break;
              case 13: _insert(_right,_down); break;
              
              case 2: _insert(_down + _xShift,_right + _yShift); break;
              case 7: _insert(_left + _yShift,_up + _xShift); break;
              
              case 5: 
                _insert(_left,_up); 
                _insert(_down,_right); 
                break;
              
              case 3: _insert(_left+_yShift,_right+_yShift); break; 
              case 6: _insert(_down + _xShift,_up + _xShift); break;
              case 9: _insert(_up,_down); break;
              case 10:      
                _insert(_left,_up); 
                _insert(_down,_right); 
                break;
              case 12: _insert(_right,_left); break;
            default: break;
            }
          });
        }
      };

      template<typename IMAGE>
      struct MarchingSquares<IMAGE,Polygons>
      {
        typedef gex::MultiPolygon output_type;
      
        template<typename STRATEGY>
        void operator()(const IMAGE& _image, STRATEGY _s, output_type& _out)
        {
          auto&& _segments = marching_squares(_image,_s);
          gex::MultiRing _rings;

          typedef gex::algorithm::strategy::Threshold join_strategy;
          join(_segments,_rings,join_strategy(0.00001));
          join(_rings,_out,join_strategy(0.00001));
        }
      };
    }
  }
}

