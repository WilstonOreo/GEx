#pragma once

#include "gex/prim/Triangle.hpp"

namespace gex
{
  namespace comp
  {
    template<typename SCALAR>
    struct TriangleMesh : prim::Primitive<base::Model<3,SCALAR>>
    {
      typedef base::Model<3,SCALAR> model_type;
      typedef base::Model<4,SCALAR> matrix_model_type;
      typedef base::Matrix<matrix_model_type> matrix_type;
      GEX_PRIMITIVE(model_type)

      typedef prim::Triangle<model_type> triangle_type;
      typedef std::vector<triangle_type> triangle_cont_type;

      TriangleMesh() : correct_(false) {}

      bool empty() const
      {
        return triangles_.empty();
      }

      void update()
      {
        if (correct_) return;
        calcBounds();
        correct_ = true;
      }

      triangle_cont_type& triangles()
      {
        correct_ = false;
        return triangles_;
      }

      TBD_PROPERTY_REF(matrix_type,matrix)
      TBD_PROPERTY_REF_RO(triangle_cont_type,triangles)
      TBD_PROPERTY_REF_RO(bounds_type,bounds)
      TBD_PROPERTY_RO(bool,correct)
    private:
      void calcBounds()
      {
        bounds_type _bounds;
        for (const auto& _t : triangles_)
          for (const auto& _p : _t.points())
            _bounds.extend(_p);
        bounds_=_bounds;
      }
    };
  }
}
