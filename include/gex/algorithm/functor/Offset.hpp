#pragma once

#include "gex/cgal.hpp"
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include "gex/algorithm/unify.hpp"
#include "gex/algorithm/convert.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace functor
      {
        template<typename PRIMITIVE>
        struct Offset
        {
        };

        template<>
        struct Offset<Ring>
        {
          void operator()(const Ring& _in, MultiPolygon& _out, Scalar _offset)
          {
            auto _cgalPrim = cgal::adapt(_in);
            ///@todo Could replace this with more accurate minkowski sum approach (CGAL sux btw)
            cgal::PolygonPtrVector _offsetPolygons;
            if (_offset < 0)
            {
              _offsetPolygons = CGAL::create_exterior_skeleton_and_offset_polygons_2(double(-_offset),_cgalPrim);
              _offsetPolygons.erase(_offsetPolygons.begin());
            }
            else
            {
              _offsetPolygons = CGAL::create_interior_skeleton_and_offset_polygons_2(double(_offset),_cgalPrim);
            }
            _out = cgal::adapt(_offsetPolygons);
          }
        };

        template<>
        struct Offset<Polygon>
        {
          void operator()(const Polygon& _in, MultiPolygon& _out, Scalar _offset)
          {
            MultiPolygon _boundary, _holes;
            Offset<Ring>()(_in.boundary(),_boundary,_offset);

            for (auto& _r : _in.holes())
            {
              MultiPolygon _newHoles;
              Offset<Ring>()(_r,_newHoles,_offset);
              _holes.insert(_holes.end(),_newHoles.begin(),_newHoles.end());
            }
            _holes = unify(_holes);
            difference(_boundary,_holes,_out);
          }
        };

        template<>
        struct Offset<MultiPolygon>
        {
          void operator()(const MultiPolygon& _in, MultiPolygon& _out, Scalar _offset)
          {
            for (auto& _p : _in)
            {
              MultiPolygon _offsetPolygons;
              Offset<Polygon>()(_p,_offsetPolygons,_offset);
              _out.insert(_out.end(),_offsetPolygons.begin(),_offsetPolygons.end());
            }
            if (_offset > 0)
            {
              _out = unify(_out);
            }
          }
        };
      }
    }
  }
