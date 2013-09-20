#pragma once

#include "gex/misc.hpp"
#include "algorithm/boost.hpp"

#include "base/Model.hpp"
#include "base/EigenTypedefs.hpp"
#include "base/Coords.hpp"
#include "base/Range.hpp"
#include "base/PolarVec.hpp"

namespace gex
{
  using base::X;
  using base::Y;
  using base::Z;

  typedef base::Axis Axis;
  typedef base::Range<Scalar> Range;
}

#include "base/Packet.hpp"
#include "base/Bounds.hpp"
#include "base/Ray.hpp"

namespace gex
{
  typedef base::Bounds<Model2> Bounds2;
  typedef base::Matrix<Model2> Matrix2;
  typedef base::Ray<Model2> Ray2;

  typedef base::Bounds<Model3> Bounds3;
  typedef base::Matrix<Model3> Matrix3;
  typedef base::Ray<Model3> Ray3;

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

#include <boost/geometry/geometries/register/box.hpp>

BOOST_GEOMETRY_REGISTER_BOX(
  gex::Bounds2,
  gex::Point2,
  min(),max())

BOOST_GEOMETRY_REGISTER_BOX(
  gex::Bounds3,
  gex::Point3,
  min(),max())
