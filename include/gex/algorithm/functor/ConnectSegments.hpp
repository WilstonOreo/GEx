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
        template<typename POINT>
        struct Junction
        {
          void operator()(const POINT& _p0, const POINT& _p1, prim::LineString<POINT>& _junction)
          {
          }
        };

        namespace 
        {
          /// If linestring is closed, it is converted into a ring, shifted and back converted into a linestring
          template<typename LINESTRING, typename LINESTRING, typename EPSILON>
          void handle_closed_linestring(const POINT& _point, LINESTRING& _lineString, EPSILON _eps = 0.000001)
          {
          }
        }


        template<typename A, typename B>
        struct Approach
        {
        };
        
        template<typename POINT>
        struct Approach<POINT,prim::Segment<POINT>>
        {
          typedef POINT point_type;
          typedef prim::Segment<POINT> segment_type;

          void operator()(const point_type& _a, segment_type& _b)
          {
            if (sqrDistance(_b[0],_a) > sqrDistance(_b[1],_a))
              std::swap(_b[0],_b[1]);
          }
        };

        template<typename POINT>
        struct Approach<prim::Segment<POINT>,POINT> 
        {
          void operator()(segment_type& _a, const point_type& _b)
          {
            Approach<POINT,prim::Segment<POINT>>()(_a,_b);
          }
        };

        template<typename POINT>
        struct Approach<POINT,prim::LineString<POINT>>
        {
          typedef POINT point_type;
          typedef prim::LineString<POINT> linestring_type;

          void operator()(const point_type& _a, linestring_type& _b)
          {
            if (sqrDistance(_b[0],_a) > sqrDistance(_b[1],_a))
              std::reverse(_b.begin(),_b.end());
            
            handle_closed_linestring(_a[0],_b);
          }
        };

        template<typename POINT>
        struct Approach<prim::LineString<POINT>,POINT> 
        {
          void operator()(prim::LineString<POINT>& _a, const POINT& _b)
          {
            Approach<POINT,prim::LineString<POINT>>()(_a,_b);
          }
        };


        template<typename POINT>
        struct Approach<prim::Segment<POINT>,prim::Segment<POINT>>
        {
          typedef prim::Segment<POINT> segment_type;
          void operator()(segment_type& _a, segment_type& _b)
          {
            int _idxA = 0, _idxB = 0;
            scalar_type _minDist = inf<scalar_type>();

            for (size_t i = 0; i < 2; ++i)
              for (size_t j = 0; j < 2; ++j)
              {
                auto _dist = sqrDistance(_a[i],_b[j]);
                if (_dist < _minDist) 
                {
                  _idxA = i; _idxB = j;
                  _minDist = _dist;
                }
              }
            if (_idxA > 0) std::swap(_a[0],_a[1]);
            if (_idxB > 0) std::swap(_b[0],_b[1]);
          }
        };
        
        template<typename POINT>
        struct Approach<prim::Segment<POINT>,prim::LineString<POINT>>
        {
          typedef prim::Segment<POINT> segment_type;
          typedef prim::LineString<POINT> linestring_type;
          
          void operator()(segment_type& _a, linestring_type& _b)
          {
            if (_b.empty()) return;
            int _idxA = 0, _idxB = 0;
            scalar_type _minDist = inf<scalar_type>();

            for (size_t i = 0; i < 2; ++i)
              for (size_t j = 0; j < 2; ++j)
              {
                auto _dist = sqrDistance(_a[i],j ? _b.back() : _b.front());
                if (_dist < _minDist) 
                {
                  _idxA = i; _idxB = j;
                  _minDist = _dist;
                }
              }
            if (_idxA > 0) std::swap(_a[0],_a[1]);
            if (_idxB > 0) std::reverse(_b.begin(),_b.end());

            handle_closed_linestring(_a[0],_b);
          }
        };

        template<typename POINT>
        struct Approach<prim::LineString<POINT>,prim::Segment<POINT>>
        {
          void operator()(linestring_type& _a, segment_type& _b)
          {
            Approach<prim::Segment<POINT>,prim::LineString<POINT>>()(_b,_a);
          }
        };
        

        template<typename POINT>
        struct Approach<POINT,prim::Ring<POINT>>
        {
        };

        template<typename POINT>
        struct Approach<prim::Segment<POINT>,prim::Ring<POINT>>
        {
        };

        template<typename POINT>
        struct Approach<prim::LineString<POINT>,prim::Ring<POINT>>
        {
        };

      }
    }
}




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
