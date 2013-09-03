#pragma once

#include <gex/comp.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>
#include <gex/io/read.hpp>

namespace create
{
  using gex::TriangleMesh;

  /// Load meshes from a list of filenames given in a text file
  std::vector<std::pair<std::string,TriangleMesh>> meshes(const std::string& _textFilename)
  {
    std::ifstream _is(_textFilename);
    std::vector<std::pair<std::string,TriangleMesh>> _meshes;
    while (_is.good())
    {
      std::string _meshFilename; 
      std::getline(_is,_meshFilename);
      boost::algorithm::trim(_meshFilename);
      if (_meshFilename.empty()) continue;
      TriangleMesh _mesh;

      if (gex::io::read(_meshFilename,_mesh))
      {
        _meshFilename = boost::filesystem::path(_meshFilename).stem().string();
        _meshes.push_back(std::make_pair(_meshFilename,_mesh));
      }
    }

    return _meshes;
  }
}
