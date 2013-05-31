#pragma once

#include <array>
#include "Pos.hpp"
#include "Cost.hpp"
#include "Candidate.hpp"
#include "gex/misc.hpp"

namespace gex
{
    namespace bsp
    {
      namespace split
      {
        template
        <
          typename BUILD_STATE,
          typename PRIM_SPLIT_POS = Pos<typename BUILD_STATE::primitive_type>,
          typename PRIM_SPLIT_COST = Cost<typename BUILD_STATE::primitive_type>,
          size_t N_BUCKETS = 8
        >
        struct CostFunction
        {
          typedef BUILD_STATE state_type;
          typedef typename state_type::primitive_type primitive_type;
          typedef typename state_type::primitive_cntr_type primitive_cntr_type;
          typedef typename state_type::node_geometry_type node_geometry_type;
          typedef typename state_type::bounds_type bounds_type;
          typedef typename state_type::scalar_type scalar_type;
          typedef base::Axis axis_type;

          CostFunction() : primitive_(nullptr) {}

          bool operator()(const state_type& _state,
                          const axis_type _axis,
                          typename node_geometry_type::splitpos_type& _splitPos)
          {
            typedef Candidate<primitive_type> SplitCandidate;

            // With this bucket structure, we can achieve building a good kdtree within O(n*log n)
            struct Buckets
            {
            private:
              // A bucket contains two split candidates: 1 left and 1 right
              struct Bucket
              {
                Bucket() : cost_(0) {}
                void put(scalar_type _pos, primitive_type const* _primitive)
                {
                  if (_pos >= leftExt_ && _pos <= rightExt_)
                  {
                    if (!left_.primitive() || _pos <= left_.pos() )  left_(_pos,_primitive);
                    if (!right_.primitive() || _pos >= right_.pos()) right_(_pos,_primitive);
                  }
                }

                TBD_PROPERTY_REF(SplitCandidate,left)
                TBD_PROPERTY_REF(SplitCandidate,right)
                TBD_PROPERTY(scalar_type,leftExt)
                TBD_PROPERTY(scalar_type,rightExt)
                TBD_PROPERTY_REF(scalar_type,cost)
              };

            public:
              Buckets(const bounds_type& _bounds, const axis_type& _bucketAxis)
              {
                axis_ = _bucketAxis;
                min_ = _bounds.min()[axis_];
                max_ = _bounds.max()[axis_];
                invSize_ = N_BUCKETS / (max_ - min_);
                scalar_type _bucketSize = (max_ - min_) / N_BUCKETS;
                scalar_type _bucketPos = min_;

                for (Bucket& _bucket : buckets_)
                {
                  _bucket.leftExt(_bucketPos);
                  _bucketPos += _bucketSize;
                  _bucket.rightExt(_bucketPos);
                }

                buckets_[0].leftExt(_bucketSize*0.5 + min_);
                buckets_[N_BUCKETS-1].rightExt(_bucketPos-_bucketSize*0.5);
              }

              void insert(primitive_type const* _p)
              {
                scalar_type _s = PRIM_SPLIT_POS()(*_p,axis_);
                Bucket* _bucket = bucketBySplitPos(_s);
                if (!_bucket) return;

                _bucket->cost() += PRIM_SPLIT_COST()(*_p);
                _bucket->put(_s,_p);
              }

              const SplitCandidate* splitCandidate()
              {
                using misc::inf;

                const SplitCandidate* _bestCandidate = nullptr;
                scalar_type _minSplitCost = inf<scalar_type>();
                scalar_type _overallCost = 0;

                for (Bucket& _bucket : buckets_)
                  _overallCost += _bucket.cost();

                scalar_type _rightCost = _overallCost, _leftCost = 0;

                // The minimum cost is the extent of bounds
                scalar_type _minCost = max_ - min_;
                scalar_type _cost = inf<scalar_type>();

                for (const Bucket& _bucket : buckets_)
                {
                  if (_bucket.left().primitive())
                  {
                    _cost = splitCost(_bucket.left().pos(),_leftCost,_rightCost);
                    if (_cost < _minSplitCost)
                    {
                      _minSplitCost = _cost;
                      _bestCandidate = &_bucket.left();
                    }
                  }

                  _leftCost += _bucket.cost();
                  _rightCost -= _bucket.cost();

                  if (_bucket.right().primitive())
                  {
                    _cost = splitCost(_bucket.right().pos(),_leftCost,_rightCost);
                    if (_cost < _minSplitCost)
                    {
                      _minSplitCost = _cost;
                      _bestCandidate = &_bucket.right();
                    }
                  }
                  if (_minSplitCost < _minCost) return nullptr;
                }

                return _bestCandidate;
              }

            private:
              std::array<Bucket,N_BUCKETS> buckets_;
              axis_type axis_;

              Bucket* bucketBySplitPos(scalar_type _s)
              {
                int _bucketIndex = invSize_ * (_s - min_);
                if (_bucketIndex < 0 || _bucketIndex >= N_BUCKETS) return nullptr;
                return &buckets_[_bucketIndex];
              }

              scalar_type splitCost(scalar_type _s,
                                    scalar_type _leftCost,
                                    scalar_type _rightCost)
              {
                return ((_s - min_)  * _leftCost + (max_ - _s) * _rightCost);
              }
              scalar_type min_, max_, invSize_;
            } _buckets(_state.nodeGeometry().bounds(),_axis);

            primitive_ = nullptr;

            /// Insert split candidates into buckets
            for ( auto const* _primitive : _state.primitives() )
              _buckets.insert(_primitive);

            const SplitCandidate* _bucketSplitCandidate = _buckets.splitCandidate();
            if (!_bucketSplitCandidate) return false;

            _splitPos = _bucketSplitCandidate->pos();
            primitive_ = _bucketSplitCandidate->primitive();
            return true;
          }

          TBD_PROPERTY(const primitive_type*,primitive)
        };
      }
    }
  }
