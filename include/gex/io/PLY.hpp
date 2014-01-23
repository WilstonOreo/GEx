#pragma once

#include <iostream>
#include <fstream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace gex
{
  namespace strategy
  {
    namespace io
    {
      struct Points3 {};
    }
  }

  namespace io
  {
    template<typename STRATEGY>
    struct PLY {};

    /// A most basic PLY implementation for reading a list of points
    template<>
    struct PLY<gex::strategy::io::Points3>
    {
      template<typename PATH, typename POINTS>
      bool operator()(const PATH& _filename, POINTS& _points)
      {
        std::ifstream _in( _filename );
        if (!_in)
        {
          std::cerr << "PLYReader: could not read file " << _filename << std::endl;
          return false;
        }

        _points.clear();

        std::string _line;
        
        enum { HEADER, DATA, END } _mode = HEADER;
        
        size_t _vertices = 0;

        while (std::getline(_in, _line))
        {
          boost::algorithm::to_lower(_line);
          boost::algorithm::trim(_line);
          std::string _token;
          std::istringstream _iss(_line);
          
          switch (_mode)
          {
            case HEADER:
            {
              _iss >> _token;
              if (_token == "end_header")
              {
                _mode = DATA;
                break;
              } else
              if (_token == "element")
              {
                _iss >> _token;
                if (_token == "vertex")
                {
                  _iss >> _vertices;
                  _points.reserve(_vertices);
                  break;
                }
              }
              break;
            }
            case DATA:
            {
              double _x, _y, _z;
              _iss >> _x >> _y >> _z;
              _points.emplace_back(_x,_y,_z);
            }
            default: break;
          };
        }

        if (_points.empty()) return false;

        return true;
      }
    };
  }
}
