#pragma once

#include "Coords.hpp"
#include "Model.hpp"
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

namespace gex
{
  namespace base
  {
    template<typename MODEL>
    using Vec = Eigen::Matrix<typename MODEL::scalar_type,MODEL::dimensions(),1>;
    
    template<typename MODEL>
    using Point = Vec<MODEL>;
  
    template<typename MODEL>
    using Matrix = Eigen::Matrix<typename MODEL::scalar_type,MODEL::dimensions(),MODEL::dimensions()>;

    template<typename VEC>
    using VecContainer = std::vector<VEC,Eigen::aligned_allocator<VEC> >;

    template<typename SCALAR>
    using Transform2 = Eigen::Transform<SCALAR,2,Eigen::Affine>;

    template<typename SCALAR>
    using Transform3 = Eigen::Transform<SCALAR,3,Eigen::Affine>;


    /// Get rotation matrix from 3-dimensional direction vector (it is assumed that it is normalized)
    template<Axis AXIS, typename VEC, typename MATRIX>
    void matrixFromVector(const VEC& _vec, MATRIX& _m)
    {
      VEC _up = VEC::Zero();
      _up(1) = 1.0;
      VEC _right = _vec.cross(_up).normalized();
      _up = _right.cross(_vec).normalized();
      _m.col(0) = _right;
      _m.col(1) = _up;
      _m.col(2) = _vec;
    }

    template<
      typename CENTER, 
      typename OFFSET, 
      typename X_ROT, 
      typename Y_ROT, 
      typename Z_ROT,
      typename SCALE>
    Transform3<typename CENTER::Scalar> transformation(
        const CENTER& _c, 
        const OFFSET& _off, 
        const X_ROT& _xDeg,
        const Y_ROT& _yDeg,
        const Z_ROT& _zDeg,
        const SCALE& _scale)
    {
      typedef CENTER vec_type;
      typedef typename vec_type::Scalar scalar_type;
      return 
        Eigen::Translation<scalar_type,3>(_c) *
        Eigen::Translation<scalar_type,3>(_off) *
        Eigen::AngleAxis<scalar_type>(deg2rad(_zDeg),vec_type::UnitZ()) *
        Eigen::AngleAxis<scalar_type>(deg2rad(_yDeg),vec_type::UnitY()) *
        Eigen::AngleAxis<scalar_type>(deg2rad(_xDeg),vec_type::UnitX()) *
        Eigen::Scaling(_scale) * 
        Eigen::Translation<scalar_type,3>(-_c);
    }

    template<
      typename X_ROT, 
      typename Y_ROT, 
      typename Z_ROT,
      typename SCALE>
    Transform3<typename SCALE::Scalar> transformation(
        const X_ROT& _xDeg,
        const Y_ROT& _yDeg,
        const Z_ROT& _zDeg,
        const SCALE& _scale)
    {
      typedef SCALE vec_type;
      typedef typename vec_type::Scalar scalar_type;
      return 
        Eigen::AngleAxis<scalar_type>(deg2rad(_zDeg),vec_type::UnitZ()) *
        Eigen::AngleAxis<scalar_type>(deg2rad(_yDeg),vec_type::UnitY()) *
        Eigen::AngleAxis<scalar_type>(deg2rad(_xDeg),vec_type::UnitX()) *
        Eigen::Scaling(_scale); 
    }

    template<typename VEC, typename FUNCTOR>
    void for_each_dim(const VEC& _vec, FUNCTOR _f)
    {
      for (size_t i = 0; i < VEC::SizeAtCompileTime; i++)
        _f(_vec(i),i);
    }
    
    template<typename VEC, typename FUNCTOR>
    void for_each_dim(VEC& _vec, FUNCTOR _f)
    {
      for (size_t i = 0; i < VEC::SizeAtCompileTime; i++)
        _f(_vec(i),i);
    }

    template<typename VEC>
    Vec<base::Model<VEC::SizeAtCompileTime-1,typename VEC::Scalar>> 
      project(const VEC& _v, Axis _axis)
    {
      typedef Vec<base::Model<VEC::SizeAtCompileTime-1,typename VEC::Scalar>> proj_vec_type;
      proj_vec_type _proj;
      size_t _dim = 0;
      gex::base::for_each_dim(_v,[&_dim,&_proj,&_axis](const typename VEC::Scalar& _s, size_t i)
      {
        if (i == _axis) return;
        _proj(_dim) = _s;
        _dim++;
      });
      return _proj;
    }

    template<typename VEC, typename SCALAR>
    Vec<base::Model<VEC::SizeAtCompileTime+1,typename VEC::Scalar>> 
      expand(const VEC& _v, Axis _axis, const SCALAR& _d)
    {
      typedef Vec<base::Model<VEC::SizeAtCompileTime+1,typename VEC::Scalar>> exp_point_type;
      exp_point_type _exp;
      int _dim = Axis::X;
      gex::base::for_each_dim(_exp,[&](typename VEC::Scalar& _s, size_t i)
      {
        if (i == _axis)
        {
          _s = _d;
          return;
        }
        _s = _v[_dim];
        _dim++;
      });
      return _exp;      
    }
    
    template<typename VEC>
    Vec<base::Model<VEC::SizeAtCompileTime+1,typename VEC::Scalar>> 
      expand(const VEC& _v)
    {
      typedef Vec<base::Model<VEC::SizeAtCompileTime+1,typename VEC::Scalar>> exp_point_type;
      exp_point_type _exp;
      gex::base::for_each_dim(_v,[&_exp](const typename VEC::Scalar& _s, size_t i)
      {
        _exp[i] = _s;
      });
      _exp[VEC::SizeAtCompileTime] = 1.0;
      return _exp;
    }
  }
}

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#define GEX_REGISTER_POINT(POINT,DIM,SCALAR)\
namespace boost { namespace geometry { namespace traits {\
BOOST_GEOMETRY_DETAIL_SPECIALIZE_POINT_TRAITS(POINT,DIM,SCALAR,cs::cartesian)\
  template<size_t dim> \
  struct access<POINT, dim>\
  {\
    static gex::Scalar get(POINT const& p)\
    { return  p(dim); }\
    static void set(POINT& p, SCALAR const& value)\
    { p(dim) =  value; } \
  };\
}}}

namespace gex
{
  typedef misc::defaults::ScalarType Scalar;
  
  typedef base::Model<2,Scalar> Model2;
  typedef base::Coords<Model2> Coords2;
  typedef base::Point<Model2> Point2;
  typedef base::Vec<Model2> Vec2;

  typedef base::Model<3,Scalar> Model3;
  typedef base::Coords<Model3> Coords3;
  typedef base::Point<Model3> Point3;
  typedef base::Vec<Model3> Vec3;
  typedef base::Matrix<Model3> Matrix3;

  typedef base::Model<4,Scalar> Model4;
  typedef base::Coords<Model4> Coords4;
  typedef base::Point<Model4> Point4;
  typedef base::Vec<Model4> Vec4;
  typedef base::Matrix<Model4> Matrix4;

  typedef base::Transform2<Scalar> Transform2;
  typedef base::Transform3<Scalar> Transform3;
}

GEX_REGISTER_POINT(gex::Vec2,2,gex::Scalar)
GEX_REGISTER_POINT(gex::Vec3,3,gex::Scalar)
GEX_REGISTER_POINT(gex::Vec4,4,gex::Scalar)
