#pragma once

#include "node/Geometry.hpp"

namespace gex
{
  namespace bsp
  {
#define GEX_BSP_NODE_TYPES(NODE)\
          typedef NODE node_type;\
          typedef typename node_type::scalar_type scalar_type;\
          typedef typename node_type::inner_type inner_node_type;\
          typedef typename node_type::leaf_type leaf_node_type;\
          typedef typename node_type::geometry_type node_geometry_type;\
          typedef typename node_geometry_type::bounds_type bounds_type;

    template
    <
    typename MODEL,
             typename INNER,
             typename LEAF
             >
    struct Node
    {
      typedef MODEL model_type;
      typedef typename model_type::scalar_type scalar_type;
      typedef scalar_type splitpos_type;
      typedef node::Geometry<model_type> geometry_type;
      typedef base::Axis axis_type;
      typedef INNER inner_type;
      typedef LEAF leaf_type;
      typedef uint32_t index_type;

      bool isLeaf() const
      {
        return !inner_.is();
      }

      inner_type& inner()
      {
        return inner_;
      }

      const inner_type& inner() const
      {
        //  GEX_ASSERT(inner_.is());
        return inner_;
      }

      leaf_type& leaf()
      {
        //  GEX_ASSERT(isLeaf());
        return leaf_;
      }

      const leaf_type& leaf() const
      {
        //   GEX_ASSERT(isLeaf());
        return leaf_;
      }

    protected:
      union
      {
        inner_type inner_;
        leaf_type leaf_;
      };
    };
  }
}
