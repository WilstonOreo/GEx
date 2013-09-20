#pragma once

#include "EigenTypedefs.hpp"

namespace gex
{
  namespace base
  {
    template<class MODEL>
    struct Ray : MODEL
    {
      GEX_MODEL(MODEL)
      typedef base::Point<MODEL> point_type;
      typedef base::Vec<MODEL> vec_type;

      Ray(const point_type _org = point_type(), const vec_type _dir = vec_type(),
          scalar_type _tNear = 0.0, scalar_type _tFar = MODEL::scalarMax()) :
        org_(_org), dir_(_dir), tNear_(_tNear), tFar_(_tFar)  {  }

      point_type point(scalar_type _t) const
      {
        return org_ + _t * dir_;
      }

      void params(point_type _org, vec_type _dir)
      {
        org_=_org;
        dir_=_dir;
      }

      TBD_PROPERTY_REF(point_type,org)
      TBD_PROPERTY_REF(vec_type,dir)
      TBD_PROPERTY_REF(scalar_type,tNear)
      TBD_PROPERTY_REF(scalar_type,tFar)
    };
  }
}
