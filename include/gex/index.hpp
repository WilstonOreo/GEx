#pragma once

#include <boost/geometry/index/rtree.hpp> 
#include "base.hpp"

namespace gex
{
  namespace index
  {
    using boost::geometry::index::intersects;
    using boost::geometry::index::within;
    using boost::geometry::index::rtree;
    using boost::geometry::index::linear;
    using boost::geometry::index::rstar;
    using boost::geometry::index::insert;
    using boost::geometry::index::query;

    static inline constexpr size_t defaultNumber() { return 8; }

    template<typename MODEL, typename VALUE>
    using RTreeNode = std::pair<base::Bounds<MODEL>,VALUE>;

    template<typename NODE, size_t NUMBER = defaultNumber()>
    using RTree = rtree<NODE,linear<NUMBER>>; 
  }

  template<typename VALUE>
  using RTree2Node = index::RTreeNode<Model2,VALUE>;

  template<typename VALUE, size_t NUMBER = index::defaultNumber()>
  using RTree2 = index::RTree<RTree2Node<VALUE>,NUMBER>;
  
  template<typename VALUE>
  using RTree3Node = index::RTreeNode<Model3,VALUE>;
  
  template<typename VALUE, size_t NUMBER = index::defaultNumber()>
  using RTree3 = index::RTree<RTree3Node<VALUE>,NUMBER>;
}
