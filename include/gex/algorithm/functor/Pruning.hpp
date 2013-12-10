#pragma once

#include "Branch.hpp"
#include <gex/algorithm/centroid.hpp>
#include <gex/algorithm/perimeter.hpp>
#include <gex/algorithm/intersection.hpp>
#include <gex/algorithm/chop.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT, typename CURVES>
      void get_curves(const prim::Ring<POINT>& _ring, CURVES& _curves)
      {
        typedef POINT point_type;
        typedef base::Curve<point_type> curve_type;
        for_each<curve_type>(_ring,[&](const curve_type& _curve)
        {
          auto _det = _curve.determinant();
          if ((_ring.location() == Ring::OUTER && _det < 0) ||
              (_ring.location() == Ring::INNER && _det > 0))
            _curves.emplace_back(new curve_type(_curve));
        });
      }

      template<typename RING, typename CURVES>
      void get_curves(const prim::Polygon<RING>& _polygon, CURVES& _curves)
      {
        for_each<RING>(_polygon,[&](const RING& _ring)
        {
          get_curves(_ring,_curves);
        });
      }
      
      template<typename SEGMENTS, typename BRANCHES>
      SEGMENTS pruned(const SEGMENTS& _medial_axis, const BRANCHES& _branches)
      {
        typedef typename BRANCHES::value_type branch_type;
        typedef typename SEGMENTS::value_type segment_type;
        typedef std::set<segment_type const*> segment_set_type;

        SEGMENTS _out;
        segment_set_type _medial_axisSet;
        for (auto& _segment : _medial_axis)
          _medial_axisSet.insert(&_segment);
        using tbd::boolean::operator|=;
        using tbd::boolean::operator-=;

        segment_set_type _branchSet;

        for (auto& _branch : _branches)
          _branchSet |= _branch->getSegments();

        _medial_axisSet -= _branchSet;
        for (auto& _segment : _medial_axisSet)
          _out.push_back(*_segment);
        return _out;
      }

      template<typename POLYGON, typename MEDIAL_AXIS>
      MEDIAL_AXIS prune_secondary(const POLYGON& _simplified, const MEDIAL_AXIS& _medial_axis) 
      {
        typedef typename MEDIAL_AXIS::value_type segment_type;
        typedef typename segment_type::point_type point_type;
        typedef Branch<segment_type> branch_type;
        typedef base::Curve<point_type> curve_type;
        typedef std::unique_ptr<curve_type> curve_ptr_type;

        MEDIAL_AXIS _pruned, _tmp = _medial_axis;
        while (_tmp.size() != _pruned.size())
        {
          std::vector<curve_ptr_type> _curves;
          get_curves(_simplified,_curves);
          auto&& _branches = get_branches(_tmp);
          std::vector<branch_type const*> _primary, _secondary;
          get_primary_and_secondary_branches(_branches,_curves,_primary,_secondary);
          _pruned = pruned(_tmp,_secondary);
          std::swap(_pruned,_tmp);
        }
        return _pruned;
      }

      template<typename POLYGON, typename FACTOR, typename MEDIAL_AXIS>
      MEDIAL_AXIS prune_primary(const POLYGON& _polygon, FACTOR _factor, const MEDIAL_AXIS& _medial_axis)
      {
        typedef typename POLYGON::point_type point_type;
        typedef prim::Segment<point_type> segment_type;
        typedef algorithm::functor::Branch<gex::Segment> branch_type;
        
        auto&& _branches = get_branches(_medial_axis);
        std::vector<branch_type const*> _branchPointers;
        for (auto& _branch : _branches) 
          _branchPointers.push_back(&_branch);

        prim::MultiLineString<point_type> _ls, _left;
        auto&& _withoutBranches = pruned(_medial_axis,_branchPointers);
        for (auto& _branch : _branches)
        {
          auto&& _branchLineString = _branch.asLineString();
          if (!_branch.freeEnd())
            _ls.push_back(_branchLineString);
          else
          {
            double _length = _factor / gex::perimeter(_branchLineString);
            if (_length >= 0.5)
            {
              continue;
            }
            std::vector<double> _markers({0.0,_length,1.0-_length,1.0});
            auto&& _lineStrings = chop(_branchLineString,_markers);
            if (_lineStrings.size() < 2) continue;
            std::cout << "Chop: " << _lineStrings.size() << std::endl;
            for (size_t i = 1; i < _lineStrings.size()-1; ++i)
            {
              auto& _l = _lineStrings[i];
              if (gex::perimeter(_l) > _factor / 1000.0) 
              gex::for_each<gex::Segment>(_l,[&](const segment_type& _segment)
              {
                _withoutBranches.push_back(_segment);
              });
            }
          }
        }

        auto&& _offset = gex::offset(_polygon,strategy::offset::Default(-_factor));
        boost::geometry::correct(_offset);
        boost::geometry::correct(_ls);
        boost::geometry::intersection(_offset,_ls,_left);

        gex::for_each<gex::Segment>(_left,[&](const segment_type& _segment)
        {
          if (gex::distance(_segment.front(),_segment.back()) > _factor / 1000.0) 
            _withoutBranches.push_back(_segment);
        });

        return _withoutBranches;
      }
    }
  }
}
