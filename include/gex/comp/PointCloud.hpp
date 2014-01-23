#pragma once

#include <gex/base.hpp>

namespace gex
{
  namespace comp
  {
    template<typename SCALAR>
    struct PointCloud
    {
      typedef SCALAR scalar_type;
      typedef base::Model<3,scalar_type> model_type;
      typedef base::Point<model_type> point_type;
      typedef base::Model<4,scalar_type> matrix_model_type;
      typedef base::Bounds<model_type> bounds_type;
      typedef std::vector<point_type> point_cont_type;
      
      PointCloud() : correct_(false) {}

      bool empty() const
      {
        return points_.empty();
      }

      void update()
      {
        if (correct_) return;
        calcBounds();
        correct_ = true;
      }

      point_cont_type& points()
      {
        correct_ = false;
        return points_;
      }
    
      TBD_PROPERTY_REF_RO(point_cont_type,points)
      TBD_PROPERTY_REF_RO(bounds_type,bounds)
      TBD_PROPERTY_RO(bool,correct)
    private:
      void calcBounds()
      {
        bounds_type _bounds;
        std::cout << _bounds << std::endl;
        for (const auto& _p : points_)
          _bounds.extend(_p);
        std::cout << bounds_ << std::endl;
        bounds_=_bounds;
      }
    };
  }
}
