#pragma once
#include <vector>
#include <string>
#include <exception>
#include "model.h"

// TODO: learn and use doxygen-styled comments

namespace sc {

  // Structure for dealing with errors in normalization
  // TODO: catch it
  class NormalizeException: public std::exception {};

  // Structure for holding a single data point
  // We could use a templated structure, but let's keep it simple
  struct Point {
      float time;
      float value; 
  };
  
  // Structure for holding all data
  struct Data {
    // raw data
    std::vector<Point> raw;

    // normalized data, we should really stream computations.
    // For the sake of simplicity, we won't
    std::vector<Point> normed;

    // update the Data::normed
    void normalize(float);
  };

  // open creates new data from a file
  Data open(Model const &);

  // return a vector of data from which a sliding window is subtracted
  std::vector<Point> normalize(std::vector<Point> const &, float);
}
