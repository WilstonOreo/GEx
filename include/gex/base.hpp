#pragma once

#include "gex/misc.hpp"
#include "algorithm/boost.hpp"

#include "base/Model.hpp"
#include "base/Packet.hpp"
#include "base/Coords.hpp"
#include "base/Vec.hpp"
#include "base/Point.hpp"
#include "base/Bounds.hpp"
#include "base/Matrix.hpp"
#include "base/Ray.hpp"

namespace gex
{
  typedef misc::defaults::ScalarType Scalar;

  using base::W;
  using base::X;
  using base::Y;
  using base::Z;

  typedef base::Axis Axis;
  typedef base::Range<Scalar> Range;

  typedef base::Model<2,Scalar> Model2;
  typedef base::Coords<Model2> Coords2;
  typedef base::Point<Model2> Point2;
  typedef base::Vec<Model2> Vec2;
  typedef base::Bounds<Model2> Bounds2;
  typedef base::Matrix<Model2> Matrix2;
  typedef base::Ray<Model2> Ray2;

  typedef base::Model<3,Scalar> Model3;
  typedef base::Coords<Model3> Coords3;
  typedef base::Point<Model3> Point3;
  typedef base::Vec<Model3> Vec3;
  typedef base::Bounds<Model3> Bounds3;
  typedef base::Matrix<Model3> Matrix3;
  typedef base::Ray<Model3> Ray3;

  typedef base::Model<4,Scalar> Model4;
  typedef base::Matrix<Model4> Matrix4;

  /// Packet types
  typedef base::Packet<4,Scalar> Packet4;
  typedef base::Range<Packet4> RangePacket4;
  typedef base::Model<3,Packet4> Model3Packet4;
  typedef base::Coords<Model3Packet4> Coords3Packet4;
  typedef base::Point<Model3Packet4> Point3Packet4;
  typedef base::Vec<Model3Packet4> Vec3Packet4;
  typedef base::Bounds<Model3Packet4> Bounds3Packet4;
  typedef base::Ray<Model3Packet4> Ray3Packet4;
}

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(
  gex::Point2,gex::Scalar,cs::cartesian,x,y,x,y)

#include <boost/geometry/geometries/register/box.hpp>

BOOST_GEOMETRY_REGISTER_BOX(
  gex::Bounds2,
  gex::Point2,
  min(),max())
