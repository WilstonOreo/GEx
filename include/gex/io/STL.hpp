#pragma once

#include <iostream>
#include <fstream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace gex
{
  namespace io
  {
    struct STL
    {
      template<typename PATH, typename TRIANGLES>
      bool operator()(const PATH& _filename, TRIANGLES& _triangles)
      {
        auto _stlType = stlType(_filename);
        if (_stlType == INVALID) return false;

        std::ifstream _in( _filename );
        if (!_in)
        {
          std::cerr << "STLReader: could not read file " << _filename << std::endl;
          return false;
        }

        bool _correct = true;
        _triangles.clear();

        switch (stlType(_filename))
        {
        case BINARY:
          _correct = readBinary(_in,_triangles);
          break;
        case ASCII:
          _correct = readAscii(_in,_triangles);
          break;
        case INVALID:
          std::cerr << "STLReader: invalid input format." << std::endl;
          return false;
        }

        if (_correct)
        {
          std::cout << "Read STL file " << _filename << " with ";
          std::cout << _triangles.size() << " triangles." << std::endl;
        }
        else
        {
          std::cerr << "Reading STL " << _filename << " failed!" << std::endl;
          _triangles.clear();
        }

        if (_in) _in.close();
        return true;
      }

    private:
      enum STLType { BINARY, ASCII, INVALID = -1 };

      template<typename PATH>
      STLType stlType(const PATH& _filename) const
      {
        std::ifstream _in( _filename );
        if (!_in)
        {
          std::cerr << "STLReader: could not read file " << _filename << std::endl;
          return INVALID;
        }

        std::string _solid;
        boost::algorithm::to_lower(_solid);
        _in >> _solid;
        _in.close();

        return (_solid == "solid") ? ASCII : BINARY;
      }

      template<typename TRIANGLES>
      bool readAscii(std::ifstream& _in, TRIANGLES& _triangles)
      {
        typedef typename TRIANGLES::value_type triangle_type;
        typedef typename triangle_type::vec_type vec_type;

        vec_type _normal;
        typedef typename vec_type::Scalar scalar_type;
        triangle_type _tri;
        std::string _token, _ignore;

        while( _in >> _token )
        {
          if (_triangles.size() % chunksize() == 0)
            _triangles.reserve(_triangles.size() + chunksize());

          if ( _in.bad() )
          {
            std::cerr << "Warning! Could not read file properly!" << std::endl;
            _in.close();
            return false;
          }

          boost::algorithm::to_lower(_token);
          boost::algorithm::trim(_token);

          // Normal found
          if (_token == "normal")
          {
            base::for_each_dim(_normal,[&_in](scalar_type& _s, size_t i) 
            {
              _in >> _s;
            });
          }

          // Triangle found
          if (_token == "loop")
          {
            for (auto& _p : _tri.points())
            {
              _in >> _ignore;
              base::for_each_dim(_p,[&_in](scalar_type& _s, size_t i) 
              {
                _in >> _s;
              });
            }
            _tri.normal(_normal);
            _triangles.push_back(_tri);
            _normal = vec_type();
          }
        }
        return true;
      }

      float readFloat(std::ifstream& _in)
      {
        constexpr size_t _n = sizeof(float);
        float _f;
        _in.read(reinterpret_cast<char*>(&_f),_n);
        return _f;
      }

      template<typename TRIANGLES>
      bool readBinary(std::ifstream& _in, TRIANGLES& _triangles)
      {
        constexpr size_t _headerSize = 80;
        char _header[_headerSize];
        _in.read(_header,_headerSize);

        typedef typename TRIANGLES::value_type triangle_type;
        typedef typename triangle_type::vec_type vec_type;

        uint32_t _numTriangles;
        _in.read(reinterpret_cast<char*>(&_numTriangles),sizeof(uint32_t));

        for (uint32_t i = 0; i < _numTriangles; ++i)
        {
          if (_triangles.size() % chunksize() == 0)
            _triangles.reserve(_triangles.size() + chunksize());

          if ( _in.bad() )
          {
            std::cerr << "Warning! Could not read file properly!" << std::endl;
            _in.close();
            return false;
          }

          triangle_type _tri;
          vec_type _normal;
          typedef typename vec_type::Scalar scalar_type;

          base::for_each_dim(_normal,[this,&_in](scalar_type& _s, size_t i)
          {
            _s = readFloat(_in);
          });

          _tri.normal(_normal);          
          for (auto& _p : _tri.points())
            base::for_each_dim(_p,[this,&_in](scalar_type& _s, size_t i)
            {
              _s = readFloat(_in);
            });

          _in.seekg(2,std::ios_base::cur);

          _triangles.push_back(_tri);
        }

        return true;
      }

      static constexpr size_t chunksize()
      {
        return 16384;
      }
    };
  }
}
