#ifndef _POINT_HPP
#define _POINT_HPP

#include "Vec.hpp"
#include "Matrix.hpp"

namespace gex
{
  namespace base
  {
    /** @brief Template class to represent a point
     * @tparam MODEL Model type
     */
    template<class MODEL>
    struct Point : public Coords<MODEL>
    {
      GEX_MODEL(MODEL)

      typedef Coords<MODEL> coords_type;
      typedef Vec<MODEL> vector_type;
      typedef Matrix<Model<dimensions()+1,scalar_type>> matrix_type;

      Point() : coords_type() {}
      Point( coords_type& p ) : coords_type( p ) {}
      Point( const coords_type& p ) : coords_type( p ) {}
      Point( scalar_type _x, scalar_type _y ) : coords_type(_x,_y) { }
      Point( scalar_type _x, scalar_type _y, scalar_type _z ) : coords_type(_x,_y,_z) { }
      Point( scalar_type _x, scalar_type _y, scalar_type _z, scalar_type _w ) : coords_type(_x,_y,_z,_w) { }

      friend Point operator*( const matrix_type& _m, const Point& _p)
      {
        Point _point;
        GEX_FOREACH_DIM(i)
        {
          for (int j = 0; j < dimensions()+1; j++)
          {
            _point[i] += (j == dimensions()) ? _m(i,j) : _m(i,j) * _p[j];
          }
        }
        return _point;
      }

      friend Point operator*( const Point& _p, const matrix_type& _m)
      {
        return _m * _p;
      }

      friend vector_type operator-( const Point& a, const Point& b)
      {
        vector_type v;
        GEX_FOREACH_DIM(i) v[i] = a[i]-b[i];
        return v;
      }

      vector_type vec() const
      {
        vector_type _v;
        GEX_FOREACH_DIM(i) _v[i] = Point::a_[i];
        return _v;
      }

      friend Point operator+( const Point& a, const Point& b)
      {
        Point p;
        GEX_FOREACH_DIM(i) p[i] = a[i] + b[i];
        return p;
      }

      template<class ALT_MODEL> friend Point operator+( const Point<ALT_MODEL>& a, const Point& b)
      {
        static_assert (ALT_MODEL::dimensions() > dimensions(), "dimension of <a> must be greater then of <b>");
        Point p(a);
        GEX_FOREACH_DIM(i) p[i] += b[i];
        return p;
      }

      friend Point operator+( const Point& a, const vector_type& b)
      {
        Point p;
        GEX_FOREACH_DIM(i) p[i] = a[i] + b[i];
        return p;
      }

      operator const scalar_type*() const
      {
        return this->a_.data();
      }

      Point operator-() const
      {
        return Point() - *this;
      }

      typedef Point<Model<dimensions()-1,scalar_type>> proj_point_type;
      typedef Point<Model<dimensions()+1,scalar_type>> exp_point_type;

      proj_point_type project(Axis _axis) const
      {
        proj_point_type _projPoint;
        int _dim = 0;
        GEX_FOREACH_DIM(i)
        {
          if (i == _axis) continue;
          _projPoint[_dim] = Point::a_[i];
          _dim++;
        }
        return _projPoint;
      }

      exp_point_type expand(Axis _axis, const scalar_type& _s) const
      {
        exp_point_type _expPoint;
        Axis _dim = X;
        GEX_FOREACH_DIM_(_expPoint,i)
        {
          if (i == _axis)
          {
            _expPoint[i] = _s;
            continue;
          }
          _expPoint[i] = Point::a_[_dim];
          _dim++;
        }
        return _expPoint;
      }

      friend std::ostream& operator<<(std::ostream& _os, const Point& _p)
      {
        return _p.format(_os);
      }

      friend std::istream& operator>>(std::istream& _is, Point& _p)
      {
        std::vector<std::string> _tokens;
        tbd::parse(_is,_tokens,"(",")",",",1);
        if (_tokens.size() != _p.size()) return _is;
        GEX_FOREACH_DIM(i) 
        {
          std::stringstream _iss(_tokens[i]);
          _iss >> _p[i];
        }
        return _is;
      }
    };
  }
}

#endif /* _POINT_HPP */
