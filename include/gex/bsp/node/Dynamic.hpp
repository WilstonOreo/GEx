#pragma once

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      template
      <
      typename MODEL,
               typename ATTRIBUTES,
               typename INNER_ATTRIBUTES,
               typename LEAF_ATTRIBUTES
               >
      struct Dynamic : Node<MODEL,uint32_t,ATTRIBUTES,INNER_ATTRIBUTES,LEAF_ATTRIBUTES>
      {
        struct Inner : base_node_type::Inner
        {
          Inner() is_(true) {}

          TBD_PROPERTY(bool,is)
          TBD_PROPERTY(float,splitPos)
          TBD_PROPERTY(base::Axis,axis)
          TBD_PROPERTY(Node*,left)
          TBD_PROPERTY(Node*,right)
        };

        struct Leaf : base_node_type::Leaf
        {
        };
      };
    }
  }
}
