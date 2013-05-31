#ifndef _VEC_HPP
#define _VEC_HPP

#include "Coords.hpp"
#include <boost/static_assert.hpp>
#include <boost/mpl/int.hpp>
#include <algorithm>

namespace gex
{
    namespace base
    {
      /** @brief Template class to represent a vector
       * @tparam MODEL Model type
       */
      template<class MODEL>
      struct Vec : Coords<MODEL>
      {
        GEX_MODEL(MODEL)
        typedef Coords<MODEL> coords_type;
        typedef Vec<MODEL> vector_type;

        Vec() : coords_type() {}
        Vec( Vec& v ) : coords_type( v ) {}
        Vec( const Vec& v ) : coords_type( v ) {}
        Vec( scalar_type _x, scalar_type _y ) : coords_type(_x,_y) { }
        Vec( scalar_type _x, scalar_type _y, scalar_type _z) : coords_type(_x,_y,_z) { }
        Vec( scalar_type _x, scalar_type _y, scalar_type _z, scalar_type _w ) : coords_type(_x,_y,_z,_w) { }

        template<typename POINT>
        Vec(POINT _p)
        {
          GEX_FOREACH_DIM(i)
          {
            this->a_[i] = _p[i]; 
          }
        }


        scalar_type sqrLength()   const
        {
          return dot(*this,*this);
        }
        scalar_type length() 		const
        {
          return    sqrt( sqrLength() );
        }
        vector_type& normalize()
        {
          scalar_type l = length();
          for(auto& a : Vec::a_) a /= l;
          return *this;
        }
        vector_type normalized() const
        {
          return *this * ( 1.0 / length() );
        }

        /** @brief Calculates cross product by this and another Vec
         * @param _left first vector
         * @param _right second vector
         * @returns cross product vector
         * @details Number of dimension must be 3!
         **/
        friend Vec cross(const Vec& _left, const Vec& _right)
        {
          BOOST_STATIC_ASSERT(dimensions() == 3);
          return Vec(_left.y() * _right.z() - _left.z() * _right.y(),
                     _left.z() * _right.x() - _left.x() * _right.z(),
                     _left.x() * _right.y() - _left.y() * _right.x() );
        }

        /** @brief Calculates dot product by this and another Vec
         * @param _left first vector
         * @param _right second vector
         * @returns Coord value
         **/
        friend scalar_type dot( const Vec& _left, const Vec& _right)
        {
          scalar_type sum = 0;
          GEX_FOREACH_DIM(i)
            sum += _left[i] * _right[i];
          return sum;
        }

        /** @brief Return axis with largest extent
         */
        Axis dominantAxis() const
        {
          Axis _domAxis = X;
          scalar_type _max = 0.0; 
          GEX_FOREACH_DIM(i)
          {
            scalar_type _abs = Vec::a_[i];
            if (_abs < 0) _abs = -_abs;
            if (_abs > _max)
            {
              _max = _abs;
              _domAxis = Axis(i);
            }
          }
          return _domAxis;
        }

        /// Vector operations
        Vec operator- () const
        {
          Vec v(*this);
          for(auto& a : v.a_) a = -a;
          return v;
        }
        void operator *= ( scalar_type f )
        {
          for(auto& a : this->a_) a *= f;
        }

        friend Vec operator*( const Vec& _a, const scalar_type f )
        {
          Vec _v(_a);
          _v *= f;
          return _v;
        }
        friend Vec operator*( const scalar_type f, const Vec& a )
        {
          return a*f;
        }
        friend Vec operator*( const Vec& a, const Vec& b)
        {
          Vec v;
          GEX_FOREACH_DIM(i) v[i] = a[i]*b[i];
          return v;
        }
        friend Vec operator-( const Vec& a, const Vec& b)
        {
          Vec v;
          GEX_FOREACH_DIM(i) v[i] = a[i]-b[i];
          return v;
        }
        friend Vec operator+( const Vec& a, const Vec& b)
        {
          Vec v;
          GEX_FOREACH_DIM(i) v[i] = a[i]+b[i];
          return v;
        }

        /*
            friend Vec      operator*( const Vec& a, const Matrix<DIMENSIONS+1,scalar_type>& M )
            {
              vector_type v;
              v.x = a.x * M.get(0,0) + a.y*M.get(1,0) + a.z*M.get(2,0) + M.get(3,0);
              v.y = a.x * M.get(0,1) + a.y*M.get(1,1) + a.z*M.get(2,1) + M.get(3,1);
              v.z = a.x * M.get(0,2) + a.y*M.get(1,2) + a.z*M.get(2,2) + M.get(3,2);
              return v;
            }
        */
      };
    }
  }
#endif /* _VEC_HPP */
