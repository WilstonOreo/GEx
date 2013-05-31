#pragma once

#include "Primitive.hpp"

namespace gex
{
    namespace prim
    {
      template<class MODEL>
      struct Vertex : public Primitive<MODEL>
      {
        GEX_PRIMITIVE(MODEL)

        Vertex(const point_type& _v = point_type(), const vec_type& _n = vec_type()) 
          : v_(_v), n_(_n)
        {}

        void operator()(const point_type& _v, const vec_type& _n = vec_type())
        {
          v_ = _v;
          n_ = _n;
        }

        bounds_type bounds() const
        {
          return bounds_type(v_,v_);
        }

        point_type center() const
        {
          return v_;
        }
        
        bool hasNormal()
        {
          return (n_.sqrLength() != 0);
        }

        TBD_PROPERTY_REF(point_type,v)
        TBD_PROPERTY_REF(vec_type,n)
      };
    }
  }
