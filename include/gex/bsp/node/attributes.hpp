#pragma once

#include <tbd/property.h>
#include <boost/cstdint.hpp>

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      /// A void attribute...
      struct EmptyAttribute {};

      /// An attribute which holds a range of indices
      struct RangeAttribute
      {
        TBD_PROPERTY(uint32_t,begin)
        TBD_PROPERTY(uint32_t,end)
      };

      /// An attribute which holds a pointer to a primitive
      template<typename PRIMITIVE>
      struct PrimitivePtrAttribute
      {
        typedef PRIMITIVE primitive_type;
        TBD_PROPERTY(const primitive_type*,primitive)
      };

      template<typename PRIMITIVE>
      struct RangeAttributeWithPrimitivePtr : PrimitivePtrAttribute<PRIMITIVE>, RangeAttribute
      {
        typedef PRIMITIVE primitive_type;
      };
    }
  }
}
