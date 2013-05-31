#pragma once 

#include "Style.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
    {
      /// Class for representing a generous SVG shape 
      struct Shape
      {
        Shape(const std::string& _name,
              const std::string& _parameter,
              const Style& _style) :
          name_(_name),
          parameter_(_parameter),
          style_(_style)
        {}

        std::string str() const 
        {
          std::stringstream ss;
          ss << "<"  << name_ << " " << parameter_ << " ";
          ss << style_;
          ss << "/>";
          return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& _os, const Shape& _shape) 
        {
          _os << _shape.str() << std::endl;
          return _os;
        }

        TBD_PROPERTY_RO(std::string,name)
        TBD_PROPERTY_RO(std::string,parameter)
        TBD_PROPERTY_RO(Style,style)
      };
    }
  }
}
