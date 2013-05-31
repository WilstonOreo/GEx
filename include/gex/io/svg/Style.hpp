#pragma once 
#include <tbd/property.h>
#include <string>
#include <iostream>
#include <sstream>

namespace gex
{
  namespace io
  {
    namespace svg
    {
      struct Style
      {
        Style(const char* _c)
        {
          this->operator()(_c);
        }

        Style(const std::string& _s)
        {
          this->operator()(_s);
        }

        operator std::string() const
        {
          return str();
        }

        void operator()(const std::string& _s)
        {
          str_ = _s;
        }

        friend std::ostream& operator<<(std::ostream& _os, const Style& _style)
        {
          _os << _style.str();
          return _os;
        }

        std::string str() const
        {
          if (str_.empty()) return std::string();
          return std::string("style=\"" + str_ + "\"");
        }

      private:
        std::string str_;
      };
    }
  }
}

