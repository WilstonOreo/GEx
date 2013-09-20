
#include <gex/io/SVG.hpp>
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
        holeFactor_(_holeFactor),
        map_(_map) {}

      template<typename A, typename B>
      void operator()(const A& _a, const B& _b)
      {

      }

    private:
      double boundaryFactor_, holeFactor_;
      const MULTIPOLYGON& map_;
    };

    namespace functor
    {
      namespace detail
      {
        template<typename PRIMITIVE>
        struct Candidate
        {
          Candidate(PRIMITIVE* _a, PRIMITIVE* _b, float _junctionLength) :
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
    };
  }
}

#include <gex/io/read.hpp>
#include <boost/filesystem.hpp>


template<typename PATH>
std::vector<std::string> filesWithExt( const PATH& _path,         // in this directory,
                                       const std::string& _fileExt ) // search for this name,
{
  using namespace boost::filesystem;
  auto&& _upperExt = boost::algorithm::to_upper_copy(_fileExt);
  std::vector<std::string> _files;
  if ( !exists( _path ) ) return _files;
  directory_iterator end_itr;
  for ( directory_iterator itr( _path );
        itr != end_itr;
        ++itr )
  {
    auto&& _upper = boost::algorithm::to_upper_copy(itr->path().string());
    if ( is_directory(itr->status()) )
    {
      auto&& _dirFiles = filesWithExt( itr->path(), _fileExt );
for (auto& _dirFile : _dirFiles)
        _files.push_back(_dirFile);
    }
    else if ( boost::algorithm::ends_with(_upper,_upperExt) )
    {
      _files.push_back(itr->path().string());
    }
  }
  return _files;
}


int main(int argc, char* argv[])
{
  if (argc < 2) return EXIT_FAILURE;

  using namespace gex;
  auto&& _wkts = filesWithExt(argv[1],".wkt");


  std::sort(_wkts.begin(),_wkts.end());
  
  size_t _number = 0;
  
  for (auto& _wkt : _wkts)
  {
    io::SVG _svg;
    MultiLineString _lineStrings;
    io::readWkt(_wkt,_lineStrings);

    auto&& _segments = convert<MultiSegment>(_lineStrings);
    
    using gex::Point2;
    gex::Bounds2 _bounds;
    gex::for_each<gex::Point2>(_lineStrings,[&](const gex::Point2& _point)
    {
      _bounds.extend(_point);
    });
    auto&& _center = _bounds.center();
    auto&& _max = (_bounds.size()(X) + _bounds.size()(Y)) / 2.0;
    Vec2 _m(_max,_max);

    _svg.buffer().fit(Bounds2(_center - _m,_center + _m));
    
    std::stringstream _ss;
    _ss << std::setw(5) << std::setfill('0') << _number; 

    gex::MultiRing _rings;

    _lineStrings.clear();

    _svg.draw(_segments,"stroke:red;fill:none");
    _svg.buffer().write(_ss.str()+"_segments.svg");
    _svg.clear();

    using namespace gex;

    typedef algorithm::strategy::DefectRings<Point2> strategy_type;
    typedef algorithm::junction::Straight<LineString,LineString> junction_type;
    
    algorithm::Join<gex::MultiSegment,gex::MultiRing,strategy_type> _join;
    _join(_segments,_rings,strategy_type(_lineStrings,0.0000000001,0.1),junction_type());
    
    _svg.draw(_rings,"stroke:green;fill:none");
    _svg.draw(_lineStrings,"stroke:orange;fill:none");
    _svg.buffer().write(_ss.str()+"_rings.svg");
//    _svg.buffer().write("JoinLineStrings_02_linestring.svg");*/
    _number++;
  }

  return EXIT_SUCCESS;
}

