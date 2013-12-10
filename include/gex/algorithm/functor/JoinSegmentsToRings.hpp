#pragma once
#include "Join.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT>
      struct Join<prim::MultiSegment<POINT>,prim::MultiRing<POINT>,strategy::join::DefectRings<POINT>>
      {
        typedef prim::LineString<POINT> linestring_type;

        template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION _j)
        {
          GEX_ASSERT(_strategy.threshold() > 0);

          // Initially, set threshold epsilon_ to zero.
          // It is increased later on to avoid producing degenerated rings caused by numerical issues
          epsilon_ = _strategy.threshold();
          initialEpsilon_ = epsilon_;

          /// 1st Pass: Connect segments to line strings
          auto&& _lineStringList = connect(_in);

          /// 2nd Pass: Connect line strings to rings and increase threshold if necessary
          while (!_lineStringList.empty())
          {
            // If the end points of a line string are equal, it is a ring
            auto _it = _lineStringList.begin();
            while ( _it != _lineStringList.end() )
            {
              auto _distance = algorithm::sqrDistance(_it->back(),_it->front());
              if (_distance < epsilon_ && _it->size() > 2)
              {
                _out.emplace_back(_it->begin(),_it->end());
                _it = _lineStringList.erase(_it);
              }
              else
              {
                ++_it;
              }
            }

            std::vector<linestring_type> _lineStrings;
            _lineStrings.assign(_lineStringList.begin(),_lineStringList.end());
            _lineStringList = connect(_lineStrings);

            // If distance of front and back point of all remaining
            // linestrings are larger than maxDist, consider them as defect rings
            if (epsilon_*epsilon_ > _strategy.maxDist())
            {
for (auto& _lineString : _lineStringList)
                _strategy.push_back(_lineString);
              break;
            }
          }
for (auto& _ring : _out) _ring.update();
        }
      private:

        template<typename PRIMITIVE>
        struct NearestResult
        {
          typedef PRIMITIVE const* primitive_ptr_type;
          NearestResult(
              double _distance,
              primitive_ptr_type _prim,
              bool _flip) :
            distance_(_distance),
            prim_(_prim),
            flip_(_flip) {}

          double distance_;
          primitive_ptr_type prim_;
          bool flip_;
        };

        template<typename PRIMITIVE, typename EXCLUDED, typename NEAREST>
        bool makeLineString(const PRIMITIVE& _primitive,
                            EXCLUDED& _excluded,
                            linestring_type& _lineString,
                            NEAREST _nearest)
        {
          if (_excluded.count(&_primitive)) return false;

          NearestResult<PRIMITIVE> _current(inf(),&_primitive,false);

          size_t i = 0;
          while (1)
          {
            _excluded.insert(_current.prim_);
            /// if flip possible, reverse _current
            /// length(*_current)*2 < length(_lineString)   // flip is possible 
            if (!_current.flip_)
            {
              for_each<POINT>(*_current.prim_,[&_lineString,this](const POINT& _point)
              {
                _lineString.push_back(_point);
              });
            } else
            {
              PRIMITIVE _prim = *_current.prim_;
              reverse(_prim);
              for_each<POINT>(_prim,[&_lineString,this](const POINT& _point)
              {
                _lineString.push_back(_point);
              });
            }

            _current = _nearest(_lineString);

            if (!_current.prim_) return _lineString.size() > 1;

            if (_current.distance_ >= epsilon_)
            {
              if (_current.distance_ < minDist_ && _current.distance_ >= initialEpsilon_)
                minDist_ = _current.distance_;

              return _lineString.size() > 1;
            }
            i++;
          }

          return false;
        }
/*

        template<typename PRIMITIVE>
        void flip(std::vector<PRIMITIVE>& _input)
        {
          typedef typename PRIMITIVE::bounds_type bounds_type;

          namespace bgi = boost::geometry::index;
          constexpr size_t _rtreeNodeSize = 16;
          typedef std::pair<bounds_type,PRIMITIVE const*> value;
          bgi::rtree< value, bgi::linear<_rtreeNodeSize> > _rtree;

          std::map<PRIMITIVE const*,scalar_type> _lengths;
          scalar_type _overallLength = 0.0;

          std::vector<value> _nearestFront, _nearestBack;
          for (auto& _prim : _input)
          {
            _rtree.insert(value(bounds_type(_prim.back(),_prim.front()),&_prim));
          
            auto _length = length(_prim);
            _lengths.insert(std::make_pair(&_prim,length(_prim)));
            _overallLength += _length;
          }

          for (auto& _prim : _input)
          {
            if (_lengths[&_prim] > _overallLength*0.5) continue;

            _rtree.query(bgi::nearest(_prim.front(),1),std::back_inserter(_nearestFront));
            _rtree.query(bgi::nearest(_prim.back(),1),std::back_inserter(_nearestBack));

            if (_nearestBack.empty() || _nearestFront.empty()) continue;

            if (_distanceSum > _flippedDistanceSum)
              reverse(_prim);
          }
        }
*/

        template<typename PRIMITIVE>
        std::list<prim::LineString<POINT>> connect(const std::vector<PRIMITIVE>& _input)
        {
          typedef prim::LineString<POINT> linestring_type;

          constexpr size_t _rtreeNodeSize = 16;
          std::list<linestring_type> _lineStringList;
          std::set<const PRIMITIVE*> _excluded;

          typedef NearestResult<PRIMITIVE> nearest_result_type;
          minDist_ = epsilon_;

          if (_input.size() < _rtreeNodeSize)
          {
            for ( const auto& _primitive : _input )
            {
              auto _nearest = [&](const linestring_type& _lineString)
              {
                nearest_result_type _result(inf(),nullptr,false);
                for (auto& _prim : _input)
                {
                  if (_excluded.count(&_prim) || (&_prim == &_primitive)) continue;
                  auto _dist = sqrDistance(_lineString.back(),_prim.front());
                  if (_dist < _result.distance_)
                  {
                    _result = nearest_result_type(_dist,&_prim,false);
                  }
                }
                return _result;
              };

              linestring_type _lineString;
              if (makeLineString(_primitive,_excluded,_lineString,_nearest))
              {
                _excluded.insert(&_primitive);
                _lineStringList.push_back(_lineString);
              }
            }
          }
          else
          {
            namespace bgi = boost::geometry::index;
            typedef std::pair<POINT,PRIMITIVE const*> value;
            bgi::rtree< value, bgi::linear<_rtreeNodeSize> > _rtree;
for (auto& _prim : _input)
            {
              _rtree.insert(value(_prim.front(),&_prim));
            }

            std::vector<value> _nearestNodes;

            auto _notExcluded = [&_excluded](const value& _v)
            {
              return _excluded.count(_v.second) == 0;
            };

            auto _nearest = [&_rtree,&_notExcluded,&_nearestNodes](const linestring_type& _lineString)
            {
              nearest_result_type _result(inf(),nullptr,false);
              _nearestNodes.clear();
              _rtree.query(bgi::nearest(_lineString.back(),1) && bgi::satisfies(_notExcluded), std::back_inserter(_nearestNodes));
              if (_nearestNodes.empty()) return _result;

              _result = nearest_result_type(sqrDistance(_nearestNodes[0].first,_lineString.back()),_nearestNodes[0].second,false);
              return _result;
            };

            for ( const auto& _primitive : _input )
            {
              linestring_type _lineString;
              if (makeLineString(_primitive,_excluded,_lineString,_nearest))
              {
                _excluded.insert(&_primitive);
                _lineStringList.push_back(_lineString);
              }
            }
          }

          epsilon_ =  minDist_+ initialEpsilon_;
          initialEpsilon_ *= 2;

          return _lineStringList;
        }

        double epsilon_, initialEpsilon_, minDist_;
      };
    }
  }
}


