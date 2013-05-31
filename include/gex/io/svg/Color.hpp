#pragma once

#include <map>
#include <sstream>
#include "gex/color.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      struct Color : Color4
      {
        typedef Color4 color_type;

        static const std::map<std::string,color_type> colormap()
        {
          const std::map<std::string,color_type> colormap_ =
          {
            { "none"    , color_type(0,0,0,0) },
            { "white"   , color_type(1,1,1,1) },
            { "silver"  , color_type(0.75,0.75,0.75,1) },
            { "gray"    , color_type(0.5,0.5,0.5) },
            { "black"   , color_type(0,0,0) },
            { "red"     , color_type(1,0,0) },
            { "maroon"  , color_type(0.5,0,0) },
            { "yellow"  , color_type(1,1,0) },
            { "olive"   , color_type(0.5,0.5,0) },
            { "orange"  , color_type(1,0.5,0) },
            { "green"   , color_type(0,0.5,0) },
            { "lime"    , color_type(0,1,0) },
            { "aqua"    , color_type(0,1,1) },
            { "teal"    , color_type(0,0.5,0.5) },
            { "blue"    , color_type(0,0,1) },
            { "navy"    , color_type(0,0,0.5) },
            { "fuchsia" , color_type(1,0,1) },
            { "purple"  , color_type(0.5,0,0.5) }
          };
          return colormap_;
        }

        Color(component_type _r, 
              component_type _g, 
              component_type _b,
              component_type _a = 1.0) :
          color_type(_r,_g,_b,_a) {}

        Color(const std::string& _name = std::string("none"))
        {
          this->operator()(_name);
        }

        std::string str() const
        {
          std::stringstream ss;
          ss << "rgba(";
          ss << color_type::r() << ",";
          ss << color_type::g() << ",";
          ss << color_type::b() << ",";
          ss << float(color_type::a()) / color_type::max() << ")";
          return ss.str();
        }

        void operator()(const std::string& _name)
        {
          if (!colormap().count(_name)) 
          {
            color_type::operator()(colormap().find("none")->second);
            return;
          }
          color_type::operator()(colormap().find(_name)->second);
        }

        friend std::ostream& operator<<(std::ostream& _os, const Color& _color)
        {
          _os << _color.str();
          return _os;
        }
      };
    }
  }
}
