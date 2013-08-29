#include <gex/algorithm/join.hpp>


namespace gex
{
  namespace algorithm
  {
    template<typename MULTIPOLYGON>
    struct MoveAround
    {
      MoveAround(double _boundaryFactor,
                 double _holeFactor,
                 const MULTIPOLYGON& _map) :
        boundaryFactor_(_boundaryFactor),
        holeFactor_(_holeFactor)
        map_(_map) {}

      template<typename A, typename B>
      void operator()(const A& _a, const B& _b)
      {

      }

    private:
      double boundaryFactor_, holeFactor_;
      const MULTIPOLYGON& map_;
    };
  }

  namespace functor
  {
    namespace detail
    {
      template<typename PRIMITIVE>
      struct Candidate
      {
        Candidate(PRIMITIVE* _a, PRIMITIVE* _b, _junctionLength) :
          a_(_a), b_(_b), junctionLength_(_junctionLength) {}

        friend bool operator<(const Candidate& _a, const Candidate& _b)
        {
          return _a.junctionLength() < _b.junctionLength();
        }

        TBD_PROPERTY(float,junctionLength)
        TBD_PROPERTY(PRIMITIVE*,a)
        TBD_PROPERTY(PRIMITIVE*,b)
      };
    }


    template<typename POINT>
    struct Join<prim::MultiSegment<POINT>,prim::LineString<POINT>,strategy::Simple>
    {
    public:
      template<typename IN, typename OUT, typename JUNCTION_FUNCTOR>
      void operator()(const IN& _in, OUT& _out, JUNCTION_FUNCTOR _j)
      {
        typedef typename IN::value_type primitive_type;
        typedef detail::Candidate<primitive_type> candidate_type;

for (auto& _prim : _in)
        {
          connect(_prim,_out,_j);
        }
      }
    };

    template<typename POINT>
    struct Join<prim::MultiLineString<POINT>,prim::LineString<POINT>> :
            Join<prim::MultiSegment<POINT>,prim::LineString<POINT>>;
  };
}
}

#include "create.hpp"

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;
  auto&& _circle = create::circle(Point2(),10,false,42);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  auto&& _circleWithHoles = create::circleWith3Holes(Point2(),7,false,12);

  _svg.draw(_star,"stroke:orange;fill:none");

  auto&& _skeleton = gex::skeleton(_star);
  _svg.draw(_skeleton,"stroke:white;fill:none");

//  auto&& _voronoi = gex::voronoi(_star);
//  _svg.draw(_voronoi,"stroke:yellow;fill:none");

  gex::Scalar _offset = 3.0;

  auto&& _inner = gex::offset(_star,-_offset/4.0);
  _svg.draw(_inner,"stroke:blue;fill:none");

  auto&& _thickened = gex::thicken(_star,_offset);
  _svg.draw(_thickened,"stroke:green;fill:none");
  _svg.buffer().write("Thicken.svg");

  return EXIT_SUCCESS;
}

