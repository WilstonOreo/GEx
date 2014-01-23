#pragma once

#include "Join.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename STRATEGY> 
        struct Connector {};

        template<typename START_POINT>
        struct Connector<strategy::join::ShortestPaths<START_POINT>>
        {
          // B is connected to A
          template<typename STRATEGY, typename A, typename B, typename JUNCTION>
          void operator()(STRATEGY&, A& _a, const B& _b, JUNCTION _j)
          {
            connect(_a,_b,_j);
          }
        };
        template<typename START_POINT,typename PRIMITIVE>
        struct Connector<strategy::join::ShortestPathsWithConnectionInfo<START_POINT,PRIMITIVE>>
        {
          // B is connected to A
          template<typename STRATEGY, typename A, typename B, typename JUNCTION>
          void operator()(STRATEGY& _strategy, A& _a, const B& _b, JUNCTION _j)
          {
            _strategy.insert(_a.size(),&_b);
            connect(_a,_b,_j);
          }
        };

        /// Generate a sorted of list of linestrings to be connected
        template<typename POINT, typename LINESTRINGS, typename LINESTRING>
        void connection_list(const POINT& _p, const LINESTRINGS& _in, std::vector<LINESTRING const*>& _connections)
        {
          typedef LINESTRING linestring_type;

          if (_in.empty()) return;

          typedef typename linestring_type::bounds_type bounds_type;
          typedef index::RTreeNode<POINT,linestring_type const*> node_type;

          index::RTree<node_type> _rtree;
          for (auto& _lineString : _in)
          {
            if (_lineString.empty()) continue;
            _rtree.insert(std::make_pair(POINT(_lineString.front()),&_lineString));
          }
 
          std::vector<node_type> _nearest;
          _rtree.query(index::nearest(_p,1),std::back_inserter(_nearest));
          if (_nearest.empty()) return;
          
          linestring_type const* _current = _nearest.front().second;  
          while (1)
          {
            _nearest.clear();
            _connections.push_back(_current); 
            _rtree.remove(node_type(_current->front(),_current));
            
            _rtree.query(index::nearest(_current->back(),1),
                std::back_inserter(_nearest));
            if (_nearest.empty()) break;

            _current = _nearest.front().second;
          }        
        }
      }

      template<typename POINT, typename START_POINT>
      struct Join<prim::MultiLineString<POINT>,prim::LineString<POINT>,
        strategy::join::ShortestPaths<START_POINT>>
      {
        typedef prim::LineString<POINT> output_type;

        template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION _j)
        {
          typedef prim::LineString<POINT> linestring_type;
          std::vector<linestring_type const*> _connections;
          _connections.reserve(_in.size());
          detail::connection_list(_strategy.point(),_in,_connections);
          for (auto& _connection : _connections)
          {
            detail::Connector<STRATEGY>()(_strategy,_out,*_connection,_j);
          }        
        }
      };

      template<typename POINT, typename START_POINT>
      struct Join<prim::MultiLineString<POINT>,prim::LineString<POINT>,
        strategy::join::ShortestPathsWithConnectionInfo<START_POINT,prim::LineString<POINT>>> :
        Join<prim::MultiLineString<POINT>,prim::LineString<POINT>,
              strategy::join::ShortestPaths<START_POINT>>
      {
        typedef prim::LineString<POINT> output_type;
      };
    }
  }
}
