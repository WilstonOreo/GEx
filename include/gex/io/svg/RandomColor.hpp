#pragma once

#include <sstream>
#include <gex/misc.hpp>

namespace gex
{
  namespace io
  {
    namespace svg
    {
      static inline std::string randomColor()
      {
        auto _rnd = [&]() -> int { return int(gex::rnd()*255); };
        std::stringstream ss;
        ss << "rgb(" << _rnd() << "," << _rnd() << "," << _rnd() << ")";
        return ss.str();
      }
    }
  }
}

