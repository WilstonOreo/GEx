#pragma once

#include <tbd/boolean.h>
#include <gex/algorithm/closestPoint.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename SEGMENT, typename POINT, typename SEGMENTS, typename CANDIDATES, typename EPSILON>
        void get_nearest_segments(
          SEGMENT const* _seg, const POINT& _p, const SEGMENTS& _segments,
          CANDIDATES& _candidates, EPSILON _eps, bool _flip = false)
        {
          if (!_seg) return;

for (auto& _segment : _segments)
          {
            if (&_segment == _seg) continue;

            auto&& _dist00 = sqrDistance(_p,_segment.p0());
            auto&& _dist01 = sqrDistance(_p,_segment.p1());

            if (_dist00 < _eps)
            {
              _candidates.emplace_back(&_segment,_flip,_dist00);
            }
            else if (_dist01 < _eps)
            {
              _candidates.emplace_back(&_segment,!_flip,_dist01);
            }
          }
        }

        template<typename SEGMENT, typename SEGMENTS, typename CANDIDATES, typename EPSILON>
        void get_nearest_segments(
          SEGMENT const* _seg, const SEGMENTS& _segments,
          CANDIDATES& _frontCandidates,
          CANDIDATES& _backCandidates, EPSILON _eps)
        {
          get_nearest_segments(_seg,_seg->p0(),_segments,_frontCandidates,_eps,true);
          get_nearest_segments(_seg,_seg->p1(),_segments,_backCandidates,_eps,false);
        }
      }

      template<typename SEGMENT>
      struct JunctionCandidate
      {
        JunctionCandidate() {}
        JunctionCandidate(SEGMENT const* _segment, bool _flip, float _distance) :
          segment_(_segment),
          flip_(_flip),
          distance_(_distance) {}

        TBD_PROPERTY_RO(SEGMENT const*,segment)
        TBD_PROPERTY_RO(bool,flip)
        TBD_PROPERTY_RO(float,distance)
      };


      template<typename SEGMENT>
      struct Branch
      {
        typedef SEGMENT segment_type;
        typedef typename segment_type::point_type point_type;
        typedef prim::LineString<point_type> linestring_type;
        typedef std::pair<segment_type const*,bool> segment_flip_type;

        Branch() : freeEnd_(false) {}
        Branch(segment_type const* _start, bool _flip = false) :
          freeEnd_(false)
        {
          segments_.emplace_back(_start,_flip);
        }

        std::set<segment_type const*> getSegments() const
        {
          std::set<segment_type const*> _result;
          for (auto& _segment : segments_)
            _result.insert(_segment.first);
          return _result;
        }

        linestring_type asLineString() const
        {
          if (segments_.empty()) return linestring_type();

          linestring_type _ls;
          for (auto& _segment : segments_)
          {
            _ls.push_back(startPoint(_segment));
          }
          _ls.push_back(endPoint());

          return _ls;
        }

        point_type startPoint() const
        {
          if (segments_.empty()) return point_type();
          return startPoint(segments_.front());

          auto& _front = segments_.front();

          return _front.second /* segment flipped? */ ?
                 _front.first->p0() : _front.first->p1();
        }

        point_type endPoint() const
        {
          if (segments_.empty()) return point_type();
          return endPoint(segments_.back());
        }

        template<typename SEGMENTS>
        bool findNext(const SEGMENTS& _medial_axis)
        {
          if (segments_.empty()) return false;

          auto _p = endPoint();
          auto _seg = segments_.back().first;

          typedef typename SEGMENTS::value_type segment_type;
          typedef JunctionCandidate<segment_type> candidate_type;
          typedef std::vector<candidate_type> candidates_type;

          candidates_type _candidates;
          detail::get_nearest_segments(_seg,_p,_medial_axis,_candidates,0.0000000001,true);

          if (_candidates.size() != 1)
          {
            freeEnd_ = _candidates.empty();
            return false;
          }

          segments_.emplace_back(_candidates.front().segment(),_candidates.front().flip());
          return true;
        }

        TBD_PROPERTY_REF(std::vector<segment_flip_type>,segments)
        TBD_PROPERTY_RO(bool,freeEnd)

      private:
        static point_type const& startPoint(const segment_flip_type& _s)
        {
          return _s.second /* segment flipped? */ ?
                 _s.first->p0() : _s.first->p1();
        }

        static point_type const& endPoint(const segment_flip_type& _s)
        {
          return _s.second /* segment flipped? */ ?
                 _s.first->p1() : _s.first->p0();
        }
      };


      template<typename SEGMENTS>
      std::vector<Branch<typename SEGMENTS::value_type>> get_branches(const SEGMENTS& _medial_axis)
      {
        typedef typename SEGMENTS::value_type segment_type;
        typedef JunctionCandidate<segment_type> candidate_type;
        typedef std::vector<candidate_type> candidates_type;
        std::vector<Branch<segment_type>> _branches;

        for (auto& _segment : _medial_axis)
        {
          candidates_type _front;
          candidates_type _back;
          detail::get_nearest_segments(&_segment,_medial_axis,_front,_back,0.0000000001);

          if (_front.empty() && _back.empty())
          {
            continue;
          }

          if (_back.empty())
          {
            _branches.emplace_back(&_segment,false);
          }
          else if (_front.empty())
          {
            _branches.emplace_back(&_segment,true);
          }
        }

        for (auto& _branch : _branches)
        {
          while (_branch.findNext(_medial_axis)) {}
        }

        return _branches;
      }

      template<typename BRANCHES, typename CURVES, typename PRIM, typename SEC>
      void get_primary_and_secondary_branches(const BRANCHES& _branches, const CURVES& _curves, PRIM& _primary, SEC& _secondary)
      {
        typedef typename BRANCHES::value_type branch_type;
        typedef typename CURVES::value_type::element_type curve_type;
        typedef typename curve_type::point_type point_type;
        typedef Scalar scalar_type;
        typedef std::pair<scalar_type,branch_type const*> dist_branch_type;
        typedef std::vector<dist_branch_type> dist_branches_type;
        std::map<curve_type*,dist_branches_type> _curveBranches;

        for (auto& _branch : _branches)
        {
          std::pair<Scalar,curve_type*> _curveMinDist(inf(), nullptr);
          for (auto& _curve : _curves)
          {
            auto&& _dist = sqrDistance(_curve->p1(),_branch.startPoint());
            if (_dist < _curveMinDist.first)
            {
              _curveMinDist = std::make_pair(_dist, _curve.get());
            }
          }

          auto&& _curve = _curveMinDist.second;
          if (!_curve) continue;

          auto&& _p = closestPoint(_branch.asLineString(),_curve->p1());
          auto&& _dist = sqrDistance(_curve->p1(),_p); 
          if (_curveBranches.count(_curve) == 0)
          {
            _curveBranches[_curve] = dist_branches_type();
          }
          _curveBranches.at(_curve).emplace_back(_dist, &_branch);
        }

        for (auto& _curveBranchesPair : _curveBranches)
        {
          auto _branchCandidates = _curveBranchesPair.second;
          if (_branchCandidates.empty()) continue;
          std::sort(_branchCandidates.begin(),_branchCandidates.end(),
                    [&](const dist_branch_type& _lhs, const dist_branch_type& _rhs) -> bool
          { return _lhs.first < _rhs.first; });

          _primary.push_back(_branchCandidates.front().second);
          if (_branchCandidates.size() >= 1)
          {
            for (auto it = ++_branchCandidates.begin(); it != _branchCandidates.end(); ++it)
              _secondary.push_back(it->second);
          }
        }
      }
    }
  }
}
