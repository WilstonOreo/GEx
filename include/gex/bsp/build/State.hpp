#pragma once

namespace gex
{
  namespace bsp
  {
    namespace build
    {
      /**@brief   BuildState is the base class for the
       *          state holder during the build process.
       * @detail  The build process holds several states, defined by tree's maximum depth.
       *          These states are stored in a stack
       *          The members depth, nodeIndex and nodeGeometry are mandatory.
       * @tparam  NODE Requires the node type as template parameter
       **/
      template<typename NODE>
      struct State
      {
        State() : depth_(0), nodeIndex_(0) {}

        GEX_BSP_NODE_TYPES(NODE)

        /**@brief Defines how to initialize the state from a certain input.
          *@param _stackPos If stackPos == 0, it setups initial state,
          *                 otherwise, it will be state of the build stack.
          */
        template<typename INPUT>
        void init(const INPUT& _input, unsigned _stackPos)
        {
        }

        /// Default behaviour of a state change when building a kdtree
        void change(const inner_node_type& _innerNode, State& _stateToPush)
        {
          /// Increase depth as we have to split the node into a subnode
          depth_++;
          nodeIndex_= _innerNode.leftIndex();

          // Initialize state to be pushed
          _stateToPush.depth(depth_);
          _stateToPush.nodeIndex(_innerNode.rightIndex());
          // Make bounds for subnodes
          nodeGeometry_.bounds().split(
            _innerNode.splitPos(),
            _innerNode.axis(),
            nodeGeometry_.bounds(),
            _stateToPush.nodeGeometry().bounds());
        }

        template<typename BUILD_POLICY>
        void setAttributes(const BUILD_POLICY& _buildPolicy)
        {
        }

        TBD_PROPERTY(unsigned,depth)
        TBD_PROPERTY(unsigned,nodeIndex)
        TBD_PROPERTY_REF(node_geometry_type,nodeGeometry)
      };
    }
  }
}
