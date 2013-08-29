#include <gex/io/SVG.hpp>

#include <gex/algorithm/step.hpp>
#include <gex/algorithm/convert.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace strategy
    {
      /// Regular sampler
      struct RegularSubsample
      {
        RegularSubsample(int _numSamples) :
          numSamples_(_numSamples) 
        {}

        TBD_PROPERTY_REF(int,numSamples)
      };
      
      /// Distance sampler
      struct DistanceSample
      {
        DistanceSample(double _distance) :
          distance_(_distance) {}

        TBD_PROPERTY_REF(double,distance)
      };

      /// Curvature sampler
      struct Curvature
      {
      };

      /// Distance and curvature
      struct DistanceAndCurvatureSampler
      {
      };
    }

    namespace functor
    {
      template<typename PRIMITIVE, typename SAMPLER>
      struct Sample {};

      template<typename POINT>
      struct Sample<prim::Ring<POINT>,strategy::RegularSubsample>
      {
        typedef prim::Ring<POINT> output_type;

        template<typename IN, typename OUT, typename STRATEGY, typename SAMPLE_FUNCTOR>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, SAMPLE_FUNCTOR f)
        {
          if (_in.empty() || _strategy.numSamples() == 0) return;
          if (_strategy.numSamples() == 1) 
          {
            _out = _in;
            return;
          }

          std::vector<double> _markers; 
          for (int i = 1; i < _strategy.numSamples(); ++i)
            _markers.push_back(double(i) / _strategy.numSamples());
        
          typedef prim::Segment<POINT> segment_type;
          _out.clear();
          for_each<segment_type>(_in,[&_markers,&_out,&f](const segment_type& _segment)
          {
            _out.push_back(_segment.front());
            f(_out.back());
            step(_segment,_markers,[&](const POINT& _point)
            {
              _out.push_back(_point);
              f(_out.back());
            });
          });
          _out.push_back(_in.back());
        }
      };

      template<typename POINT>
      struct Sample<prim::Ring<POINT>,strategy::DistanceSample>
      {
        typedef prim::MultiPoint<POINT> output_type;

        template<typename IN, typename OUT, typename STRATEGY, typename SAMPLE_FUNCTOR>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, SAMPLE_FUNCTOR f)
        {
          if (_in.empty() || _strategy.distance() <= 0) return;
          auto _length = perimeter(_in);

          int _num = _length / _strategy.distance();
          double _steps = _length / _num / _strategy.distance() / _num;
          std::vector<double> _markers; 
          _markers.reserve(_num);
          double _s = 0;
          for (int i = 0; i < _num; ++i)
          {
            _markers.push_back(_s);
            _s += _steps;
          }
          step(_in,_markers,[&_out,&f](const POINT& _point)
          {
            _out.push_back(_point);
          });
        }
      };
    }

    using functor::Sample;
  
    template<typename IN, typename OUT, typename STRATEGY, typename SAMPLE_FUNCTOR>
    void sample(const IN& _in, OUT& _out, STRATEGY _strategy, SAMPLE_FUNCTOR f)
    {
      Sample<IN,STRATEGY>()(_in,_out,_strategy,f);
    }
    
    template<typename IN, typename STRATEGY, typename SAMPLE_FUNCTOR>
    typename Sample<IN,STRATEGY>::output_type sample(const IN& _in, STRATEGY _strategy, SAMPLE_FUNCTOR f)
    {
      typename Sample<IN,STRATEGY>::output_type _out;
      sample(_in,_out,_strategy,f);
      return _out;
    } 
  }

  using algorithm::sample;
}


#include "create.hpp"

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;
  auto&& _circle = create::circle(Point2(),10,false,42);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);

  auto&& _star = create::star(Point2(),7,3,false,12);
  _svg.draw(_star,"stroke:orange;fill:none");

  auto _f = [](Point2& _p) {};
  auto&& _subsampled = gex::sample(_star,gex::algorithm::strategy::RegularSubsample(4),_f);
  
  auto _segments = gex::convert<gex::MultiSegment>(_subsampled);
  _svg.draw(_segments,"stroke:red;fill:none");

  auto&& _distanceSamples = gex::sample(_star,gex::algorithm::strategy::DistanceSample(4),_f);
  std::cout << _distanceSamples.size() << std::endl;
  _svg.draw(_distanceSamples,"stroke:yellow;fill:none");

  _svg.buffer().write("Sample_regular.svg");

  return EXIT_SUCCESS;
}

