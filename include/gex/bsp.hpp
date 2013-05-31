#pragma once

#include "bsp/Tree.hpp"
#include "bsp/build/Policy.hpp"
#include "bsp/build/State.hpp"
#include "bsp/build/StateWithPrimitives.hpp"
#include "bsp/Node.hpp"
#include "bsp/node/Static.hpp"
#include "bsp/node/Geometry.hpp"
#include "bsp/node/attributes.hpp"
#include "bsp/Visitor.hpp"

#include "bsp/container/Static.hpp"
#include "bsp/container/StaticWithPrimitives.hpp"
#include "bsp/split/DominantAxis.hpp"
#include "bsp/split/Candidate.hpp"
#include "bsp/split/Half.hpp"
#include "bsp/split/CostFunction.hpp"

namespace gex
{
  namespace
  {
    template<typename PRIMITIVE>
    using KDTreeNode =
      bsp::node::Static<
      typename PRIMITIVE::model_type,
      bsp::node::EmptyAttribute,
      bsp::node::RangeAttribute>;

    template<
    typename PRIMITIVE,
             typename PRIM_NODE_INTERSECTION = bsp::node::Intersection<PRIMITIVE>,
             typename PRIM_BOUNDS_EXT = bsp::node::BoundsExtension<PRIMITIVE>>
             using KDTreeBuildState =
             bsp::build::StateWithPrimitives<
             KDTreeNode<PRIMITIVE>,
             PRIMITIVE,
             bsp::build::NoExclusion,
             PRIM_NODE_INTERSECTION,
             PRIM_BOUNDS_EXT>;
  }

  template<
  typename PRIMITIVE,
           typename PRIM_NODE_INTERSECTION = bsp::node::Intersection<PRIMITIVE>,
           typename PRIM_BOUNDS_EXT = bsp::node::BoundsExtension<PRIMITIVE>,
           typename PRIM_SPLIT_POS = bsp::split::Pos<PRIMITIVE>,
           typename PRIM_SPLIT_COST = bsp::split::Cost<PRIMITIVE> >
           using KDTree =
           bsp::Tree<
           bsp::container::StaticWithPrimitives<KDTreeNode<PRIMITIVE>,PRIMITIVE>,
           bsp::build::Policy<
           KDTreeBuildState<PRIMITIVE>,
           bsp::split::DominantAxis,
           bsp::split::CostFunction<
           KDTreeBuildState<PRIMITIVE>,
           PRIM_SPLIT_POS,
           PRIM_SPLIT_COST>
           >
           >;
}
