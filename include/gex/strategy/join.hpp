#pragma once

namespace gex
{
  namespace strategy
  {
    namespace join
    {
      struct Simple {};

      struct Threshold 
      {
        Threshold(double _threshold) :
          threshold_(_threshold) {}
      
        TBD_PROPERTY_REF(double,threshold)
      };

      struct ThresholdWithReverse : Threshold 
      {
        ThresholdWithReverse(double _threshold) :
          Threshold(_threshold) {}
      };

      template<typename POINT>
      struct DefectRings : Threshold
      {
        typedef prim::MultiLineString<POINT> output_type;

        DefectRings(output_type& _out, double _threshold, double _maxDist) : 
          Threshold(_threshold),
          maxDist_(_maxDist),
          out_(_out) {}

        double const& maxDist() const
        {
          return maxDist_;
        }

        template<typename LINESTRING>
        void push_back(const LINESTRING& _lineString)
        {
          out_.push_back(_lineString);
        }

      private:
        double maxDist_;
        output_type& out_;
      };    

      template<typename POINT>
      struct ShortestPaths 
      {
        typedef POINT point_type;
        ShortestPaths(const POINT& _point) :
          point_(_point) {}

        TBD_PROPERTY_REF(point_type,point)
      };

      template<typename PRIMITIVE>
      struct ConnectionInfo
      {
        typedef PRIMITIVE primitive_type;

        ConnectionInfo() {}
        ConnectionInfo(size_t _id, primitive_type const* _prim) :
          id_(_id),
          primitive_(_prim) {}

        TBD_PROPERTY(size_t,id)
        TBD_PROPERTY(primitive_type const*,primitive)
      };

      template<typename POINT, typename PRIMITIVE>
      struct ShortestPathsWithConnectionInfo : ShortestPaths<POINT>
      {
        typedef PRIMITIVE primitive_type;
        typedef ConnectionInfo<PRIMITIVE> connectioninfo_type;
        typedef std::vector<connectioninfo_type> info_type; 

        ShortestPathsWithConnectionInfo(const POINT& _point, info_type& _info) : 
          ShortestPaths<POINT>(_point),
          info_(_info)
        {}

        info_type& info()
        {
          return info_;
        }

        info_type const& info() const
        {
          return info_;
        }

        template<typename...ARGS>
        void insert(ARGS&&..._args)
        {
          info_.emplace_back(_args...);
        }

      private:
        info_type& info_;
      };
    }
  }
}
