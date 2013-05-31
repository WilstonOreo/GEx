#pragma once

#include <vector>

namespace gex
{
    namespace bsp
    {
      namespace container
      {
        template<typename NODE, size_t MAX_DEPTH = 16>
        struct Static
        {
          GEX_BSP_NODE_TYPES(NODE)

          typedef std::vector<NODE> node_cntr_type;

          static constexpr size_t maxDepth() { return MAX_DEPTH; }

          bool empty() const
          {
            return nodes_.empty();
          }

          void clear()
          {
            nodes_.clear();
          }

          template<typename BUILD_STATE>
          void init(const BUILD_STATE& _state)
          {
            nodes_.resize(1);
          }

          template<typename BUILD_STATE>
          inner_node_type& insertInner(const BUILD_STATE& _state)
          {
            return nodes_[_state.nodeIndex()].inner();
          }

          template<typename BUILD_STATE>
          leaf_node_type& insertLeaf(const BUILD_STATE& _state)
          {
            nodes_.resize(nodes_.size()+1);
            return  nodes_.back().leaf();
          }

          const node_type& root() const
          {
            return nodes_.front();
          }

          node_type& root()
          {
            return nodes_.front();
          }

          const node_type& getNode(
              typename node_type::index_type _nodeIndex) const
          {
            return nodes_[_nodeIndex];
          }

          node_type& getNode(
              typename node_type::index_type _nodeIndex)
          {
            return nodes_[_nodeIndex];
          }

          const node_type& leftChild(const node_type& _node) const
          {
            return nodes_[_node.inner().leftIndex()];
          }
          
          const node_type& rightChild(const node_type& _node) const
          {
            return nodes_[_node.inner().rightIndex()];
          } 

          node_type const* leftChild(node_type const* _node) const
          {
            return &nodes_[_node->inner().leftIndex()];
          }

          node_type const* rightChild(node_type const* _node) const
          {
            return &nodes_[_node->inner().rightIndex()];
          }




        protected:
          node_cntr_type nodes_;
        };
      }
    }
  }

