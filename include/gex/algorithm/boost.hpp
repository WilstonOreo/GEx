#pragma once

#include "gex/misc/wrap_boost_geometry_begin.hpp"
#include <boost/geometry.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/geometry/algorithms/simplify.hpp>
#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/algorithms/sym_difference.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <boost/geometry/algorithms/num_points.hpp>
#include "gex/misc/wrap_boost_geometry_end.hpp"

namespace gex
{
  namespace algorithm
  {
    using boost::geometry::correct;
    using boost::geometry::equals;
    using boost::geometry::convex_hull;
    using boost::geometry::simplify;
    using boost::geometry::area;
    using boost::geometry::disjoint;
    using boost::geometry::num_points;
  }
}
