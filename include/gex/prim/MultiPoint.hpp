#pragma once

#include <gex/base.hpp>

namespace gex
{
  namespace prim
  {
    template<typename POINT>
    struct MultiPoint : std::vector<POINT>
    {
      typedef POINT point_type;
      typedef base::Model<POINT::SizeAtCompileTime,typename POINT::Scalar> model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef base::Range<scalar_type> range_type;
      typedef std::vector<point_type> ctnr_type;
      typedef base::Bounds<model_type> bounds_type;
    };
  }
}
