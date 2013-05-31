#pragma once

#include <cstdint>

namespace gex
{
  namespace bsp
  {
    template<typename STATE>
    struct Visitor
    {
      typedef STATE state_type;

      enum WhichNodes
      {
        LEFT = 0, LEFT_RIGHT = 1, RIGHT = 2, RIGHT_LEFT = 3
      };

      /// Returns true if the root node of the tree was traversed
      template<typename TREE>
      bool root(const TREE& _tree)
      {
        return false;
      }

      /// Returns which nodes are travered and in which order
      template<typename TREE, typename INNERNODE>
      WhichNodes inner(const TREE& _tree,
                       const INNERNODE& _node,
                       state_type& _stateToPush)
      {
        return LEFT_RIGHT;
      }

      /// Returns true if traversal can be stopped after processing the current leaf
      template<typename TREE, typename LEAFNODE>
      bool leaf(const TREE& _tree, const LEAFNODE& _node)
      {
        return false;
      }

      TBD_PROPERTY_REF(state_type,state)
    };
  }
}
