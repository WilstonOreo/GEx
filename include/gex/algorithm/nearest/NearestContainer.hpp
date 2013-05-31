#pragma once

namespace gex
{
  namespace algorithm
  {
    namespace nearest
    {
      template<typename PRIMITIVE>
      struct NearestContainer
      {
        typedef PRIMITIVE primitive_type;
        typedef typename primitive_type::scalar_type scalar_type;
        typedef std::pair<scalar_type,const primitive_type*> result_type;

        NearestContainer()
        {
          clear();
        }

        void clear()
        {
          nearest_.first = misc::inf<scalar_type>();
          nearest_.second = nullptr;
        }

        scalar_type minDistance() const
        {
          return nearest_.first;
        }

        void insert(scalar_type _distance, const primitive_type* _primitive)
        {
          nearest_ = result_type(_distance,_primitive);
        };

        TBD_PROPERTY_REF_RO(result_type,nearest)
      };
    }
  }
}
