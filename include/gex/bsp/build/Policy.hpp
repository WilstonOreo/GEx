#pragma once

#include <tbd/property.h>

namespace gex
{
  namespace bsp
  {
    namespace build
    {
      /**@brief A BuildPolicy defines a function for split and
       *        can optionally hold multiple attributes
       * @tparam STATE State holder struct for each step
       * @tparam SPLIT_AXIS Functor which defines how to choose the split axis
       * @tparam SPLIT_POS Functor which defines how to choose the split position
       **/
      template
      <
        typename STATE,
        typename SPLIT_AXIS,
        typename SPLIT_POS
      >
      struct Policy
      {
        typedef STATE state_type;
        typedef SPLIT_AXIS splitaxis_type;
        typedef SPLIT_POS splitpos_type;
        typedef typename state_type::node_geometry_type node_geometry_type;

        /**@brief Split function, returns true if node can be split.
         * @detail Returns false if node cannot be split and node
         *         will be leaf node consequently
         */
        bool split(state_type& _state, node_geometry_type& _nodeGeometry)
        {
          auto _axis = splitAxis_(_state);
          typename node_geometry_type::splitpos_type _splitPos;
          if (splitPos_(_state,_axis,_splitPos))
          {
            _nodeGeometry.splitPos(_splitPos);
            _nodeGeometry.axis(_axis);

            // Only change state's attributes if there is a split
            _state.setAttributes(*this);
            return true;
          }
          return false;
        }

        TBD_PROPERTY_REF(splitpos_type,splitPos)
        TBD_PROPERTY_REF(splitaxis_type,splitAxis)
      };
    }
  }
}

