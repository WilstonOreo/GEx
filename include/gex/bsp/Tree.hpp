#pragma once

#include "Node.hpp"
#include <array>

namespace gex
{
  namespace bsp
  {
    template
    <
    typename NODE_CONTAINER,
             typename BUILD_POLICY
             >
    struct Tree
    {
      /// Node container types
      typedef Tree<NODE_CONTAINER,BUILD_POLICY> type;
      typedef NODE_CONTAINER node_cntr_type;
      typedef BUILD_POLICY build_policy_type;

      GEX_BSP_NODE_TYPES(typename node_cntr_type::node_type)

      Tree() {}

      template<typename INPUT>
      Tree(const INPUT& _input)
      {
        build(_input);
      }

      bool empty() const
      {
        return nodes_.empty();
      }

      void clear()
      {
        nodes_.clear();
      }

      /// Iterative traversal with visitor
      template<typename VISITOR>
      bool traversal(VISITOR& _visitor) const
      {
        if (!_visitor.root(*this)) return false;

        typedef typename VISITOR::state_type state_type;
        typedef std::pair<node_type const*,state_type> stack_element;

        std::array<stack_element,NODE_CONTAINER::maxDepth()> _stack;
        auto _stackIt = _stack.begin()-1;
        auto const* _node = &nodes().root();
        bool _found = false;

        while (1)
        {
          while (!_node->isLeaf())
          {
            auto _whichNodes = _visitor.inner(*this,_node->inner(),(_stackIt+1)->second);
            switch(_whichNodes)
            {
            case VISITOR::LEFT:
              _node = nodes().leftChild(_node);
              continue;
            case VISITOR::RIGHT:
              _node = nodes().rightChild(_node);
              continue;
            case VISITOR::LEFT_RIGHT:
              ++_stackIt;
              _stackIt->first = nodes().rightChild(_node);
              _node = nodes().leftChild(_node);
              continue;
            case VISITOR::RIGHT_LEFT:
              ++_stackIt;
              _stackIt->first = nodes().leftChild(_node);
              _node = nodes().rightChild(_node);
              continue;
            }
          }

          _found |= _visitor.leaf(*this,_node->leaf());
          if (_found) return true;

          // Nothing left to do
          if (_stackIt < _stack.begin()) return _found;

          std::cout << _stackIt << " " << _stackIt->first << std::endl;

          _node = _stackIt->first;
          _visitor.state(_stackIt->second);
          --_stackIt;
        }
        return _found;
      }

      template<typename INPUT>
      void build(const INPUT& _input)
      {
        build_policy_type _buildPolicy;

        typedef typename build_policy_type::state_type BuildState;
        typedef std::array<BuildState,NODE_CONTAINER::maxDepth()> BuildStack;

        BuildStack _stack;
        typename BuildStack::iterator _stackIt = _stack.begin();

        // Declare state and reserve memory for each stack item
        int _stackItemIndex = 0;

        for ( BuildState& _stackItem : _stack)
        {
          _stackItem.init(_input,_stackItemIndex);
          ++_stackItemIndex;
        }
        BuildState& _state = _stack.front();
        bounds_ = _state.nodeGeometry().bounds();

        /// Initialize node container
        nodes_.init(_state);

        while (1)
        {
          while (_state.depth() < nodes_.maxDepth())
          {
            // Break if node can't be split
            if (!_buildPolicy.split(_state,_state.nodeGeometry())) break;

            // Make an inner node
            inner_node_type& _innerNode = nodes_.insertInner(_state);

            // Change state
            ++_stackIt;
            BuildState& _right = *_stackIt;
            _state.change(_innerNode,_right);
          }
          // We have a leaf node!

          nodes_.insertLeaf(_state);

          // Nothing left to do
          if (_stackIt == _stack.begin()) return;


          _stack[0] = *_stackIt;
          --_stackIt;
        }

      }

      TBD_PROPERTY_REF(bounds_type,bounds)
      TBD_PROPERTY_REF_RO(node_cntr_type,nodes)
    };
  }
}

