#pragma once

#include <boost/geometry/index/rtree.hpp>
#include <gex/algorithm/num.hpp>
#include "prim.hpp"

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

    template<typename PRIMITIVE>
    struct Indexable
    {
      typedef typename PRIMITIVE::bounds_type type;

      type operator()(const PRIMITIVE& _primitive)
      {
        return _primitive.bounds();
      }
    };

    template<typename MODEL>
    struct Indexable<base::Point<MODEL>>
    {
      typedef base::Point<MODEL> type;
    
      type operator()(const base::Point<MODEL>& _point)
      {
        return _point;
      } 
    };

    template<typename INDEXABLE, typename VALUE>
    using RTreeNode = std::pair<INDEXABLE,VALUE>;

    template<typename PRIMITIVE, typename VALUE = PRIMITIVE>
    using RTreeNodeWithPrimitives = 
      RTreeNode<typename Indexable<PRIMITIVE>::type,VALUE>;

    template<typename NODE, size_t NUMBER = defaultNumber()>
    using RTree = rtree<NODE,linear<NUMBER>>; 

    template<typename PRIMITIVE, size_t NUMBER =defaultNumber()>
    using RTreeWithPrimitives = 
      RTree<RTreeNode<typename PRIMITIVE::model_type,PRIMITIVE>,NUMBER>;
  
    template<typename SUB_PRIMITIVE, typename PRIMITIVE, typename RTREE, typename VALUE = SUB_PRIMITIVE>
    void constructRTree(const PRIMITIVE& _prim, RTREE& _rtree)
    {
      std::vector<RTreeNodeWithPrimitives<SUB_PRIMITIVE,VALUE>> _values;
      _values.reserve(algorithm::num<SUB_PRIMITIVE>(_prim));
      algorithm::for_each<SUB_PRIMITIVE>(_prim,[&](const SUB_PRIMITIVE& _s)
      {
        _values.emplace_back(Indexable<SUB_PRIMITIVE>()(_s),_s);
      });
      _rtree.insert(_values.begin(),_values.end());
    }

    template<typename SUB_PRIMITIVE, typename PRIMITIVE, size_t NUMBER = gex::index::defaultNumber()>
    RTreeWithPrimitives<SUB_PRIMITIVE,NUMBER> constructRTree(const PRIMITIVE& _prim)
    {
      RTreeWithPrimitives<SUB_PRIMITIVE,NUMBER> _rtree;
      constructRTree<SUB_PRIMITIVE>(_prim,_rtree);
      return _rtree;
    }
  }

  template<typename VALUE>
  using RTree2Node = index::RTreeNode<Bounds2,VALUE>;

  template<typename VALUE, size_t NUMBER = index::defaultNumber()>
  using RTree2 = index::RTree<RTree2Node<VALUE>,NUMBER>;
  
  template<typename VALUE>
  using RTree3Node = index::RTreeNode<Bounds3,VALUE>;
  
  template<typename VALUE, size_t NUMBER = index::defaultNumber()>
  using RTree3 = index::RTree<RTree3Node<VALUE>,NUMBER>;
}
