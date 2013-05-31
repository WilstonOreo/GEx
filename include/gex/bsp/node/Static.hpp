#pragma once
#include <tbd/bit.h>
#include "Geometry.hpp"

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      namespace
      {
        namespace bit=tbd::bit;

        template<typename ATTRIBUTES>
        struct Inner : ATTRIBUTES
        {
          typedef ATTRIBUTES attr_type;
          typedef base::Axis axis_type;
          typedef uint32_t index_type;

          inline bool is() const
          {
            return bit::get<bool>(data_,31,1);
          }

          inline axis_type axis() const
          {
            return bit::get<axis_type>(data_,0,2);
          }

          /// Set values
          inline Inner& setup(
            index_type _index,
            axis_type _axis,
            float _splitPos,
            attr_type _attributes = attr_type())
          {
            bit::set(data_,true,31,1);
            bit::set(data_,_index,2,29);
            bit::set(data_,_axis,0,2);
            splitPos_ = _splitPos;
            return *this;
          }

          index_type leftIndex() const
          {
            return bit::get<index_type>(data_,2,29);
          }

          index_type rightIndex() const
          {
            return leftIndex() + 1;
          }

        private:
          index_type data_;
          TBD_PROPERTY_RO(float,splitPos)
        };

        template<typename ATTRIBUTES>
        struct Leaf : ATTRIBUTES
        {
          typedef ATTRIBUTES attr_type;
        };
      }

      template
      <
      typename MODEL,
               typename INNER_ATTRIBUTES,
               typename LEAF_ATTRIBUTES
               >
      using Static =
        Node<MODEL,Inner<INNER_ATTRIBUTES>,Leaf<LEAF_ATTRIBUTES>>;
    }
  }
}
