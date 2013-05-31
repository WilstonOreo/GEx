#pragma once

#include <map>
#include "Color.hpp"

namespace gex
{
  namespace color
  {
    template<typename MODEL, typename CALC_T=float>
    struct Gradient :
        std::map<SCALAR,Color<MODEL>>
    {
      GEX_MODEL(MODEL)
      typedef point_type color_type;
      typedef CALC_T calc_type;
      color_type operator()(const scalar_type& _s) const
      {
        auto _l=begin(),_r=begin();
        for( auto _it=begin(); _it!=end(); _it++ )
        {
          if( _it->first() <= _s )
          {
            _l = _it++;
            if( _it != end() )
              _r = _it;
            else
              _r = _l;
            break;
          }
        }
        if( _r->first == _l->first )
          return _l->second;
        else
        {
          calc_type f=(calc_type)(_s-_l->first)/(calc_type)(_r->first-_l->first);
          color_type result;
          for(int i=0; i<dimensions(); ++i )
            result[i] = _l.second[i] + (scalar_type)(f*(calc_type)(_r.second[i] - _l.second[i]));
          return result;
        }
      }
    };
  }
}
