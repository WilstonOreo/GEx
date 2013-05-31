#pragma once

#include "gex/prim.hpp"
#include "gex/algorithm/sqrDistance.hpp"
#include "gex/algorithm/Nearest.hpp"

namespace gex
{
  namespace algorithm
    {
      namespace functor
      {
        struct ConnectSegments
        {
          template<typename SEGMENTS, typename RINGS>
          void operator()(const SEGMENTS& _segments,
                          RINGS& _rings,
                          double _epsilon = 0.0000000001)
          {
            GEX_ASSERT(_epsilon > 0);

            // Initially, set threshold epsilon_ to zero.
            // It is increased later on to avoid producing degenerated rings caused by numerical issues
            epsilon_ = _epsilon;
            initialEpsilon_ = epsilon_;

            /// 1st Pass: Connect segments to line strings
            auto&& _lineStringList = connect(_segments);

            /// 2nd Pass: Connect line strings to rings and increase threshold if necessary
            while (!_lineStringList.empty())
            {
              // If the end points of a line string are equal, it is a ring
              auto _it = _lineStringList.begin();
              while ( _it != _lineStringList.end() )
              {
                auto _distance = algorithm::sqrDistance(_it->back(),_it->front());
                if (_distance < epsilon_)
                {
                  _rings.push_back(Ring(*_it));
                  _it = _lineStringList.erase(_it);
                }
                else
                {
                  ++_it;
                }
              }

              std::vector<LineString> _lineStrings;
              _lineStrings.assign(_lineStringList.begin(),_lineStringList.end());
              _lineStringList = connect(_lineStrings);
            }
            for (auto& _r : _rings) _r.update();
          }

        private:

          template<typename PRIMITIVE, typename NEAREST>
          bool makeLineString(const PRIMITIVE& _primitive,
                              NEAREST& _nearest,
                              LineString& _lineString)
          {
            if (_nearest.excluded().count(&_primitive)) return false;

            const PRIMITIVE* _current = &_primitive;
            std::set<const PRIMITIVE*> _found;

            while (1)
            {
              _lineString.append(*_current);
              auto _nearestResult = _nearest.find(*_current);
              _found.insert(_current);

              _current = _nearestResult.second;
              if (!_current) return (_lineString.size() > 1);

              auto& _nearestDist = _nearestResult.first;

              if (_nearestDist < minDist_ && _nearestDist > epsilon_ &&
                  _nearestDist >= initialEpsilon_) minDist_ = _nearestDist;

              if ((_nearestDist >= epsilon_ ||
                   _found.count(_current)) && (_lineString.size() > 1))
              {
                for (auto _p : _found) _nearest.excluded().insert(_p);
                return true;
              }
            }

            return false;
          }

          template<typename PRIMITIVE>
          std::list<LineString> connect(const std::vector<PRIMITIVE> _input)
          {
            std::list<LineString> _lineStringList;
            algorithm::Nearest<PRIMITIVE> _nearest(_input);

            minDist_ = epsilon_;
            for ( const auto& _primitive : _input )
            {
              LineString _lineString;
              if (makeLineString(_primitive,_nearest,_lineString))
                _lineStringList.push_back(_lineString);
            }

            epsilon_ =  minDist_+ initialEpsilon_;
            initialEpsilon_ *= 2;

            return _lineStringList;
          }

          double epsilon_, initialEpsilon_, minDist_;
        };
      };
    }
  }
