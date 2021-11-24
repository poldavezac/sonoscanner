#include <limits>
#include <assert.h>
#include <boost/filesystem.hpp>
#include "data.h"


namespace sc { namespace{
  // private functions for opening 7z files
  namespace ext7z{
    constexpr bool _isfile(std::string const &) { return false; }  // TODO
    std::vector<Point> _open(std::string const &, float) { return {}; }    // TODO
  }

  // private functions for opening text files
  namespace exttxt{
    // simple test, not even worrying about contents
    bool _isfile(std::string const & path) {
      return boost::filesystem::extension(path) == ".txt";
    };

    // opens a txt file and returns data, given a rate and initial time
    // ignores lines starting with '#'
    // TODO: add safeguards, strip lines, verify that we are dealing with uint16_t ...
    std::vector<Point> _open(std::string const &path, float rate, float start = 0.f)
    {
      std::vector<Point> data;
      std::ifstream infile(path);
      std::string line;
      while (std::getline(infile, line))
      {
        if(line.front() == '#')
          continue;

        auto value = std::stoi(line);
        if(!(value < 0 || value > std::numeric_limits<uint16_t>::max()))
          data.push_back(Point{start, float(value)});

        // values count whether or not they have the right range.
        start += rate;
      }
      return data;
    }
  }

}}

namespace sc {
  // return the rolling mean over a given size
  // We expect the data to be sorted by time
  std::vector<Point> normalize(std::vector<Point> const & raw, float window)
  {
    if(raw.size() == 0lu)
      return {};

    auto   init   = raw.begin();
    auto   cur    = raw.begin();
    auto   last   = raw.begin();
    int    cnt    = 0lu;
    double total  = 0.0;

    std::vector<Point> normed;
    normed.reserve(raw.size());
    for(auto end = raw.end(); cur != end; ++cur)
    {
      // remove points below the window limit
      while(init != cur)
      {
        if(init->time > cur->time)
          // vector is not sorted by time
          throw NormalizeException();
        if(init->time >= cur->time - window)
          break;
        --cnt;
        total -= (double) init->value;
      }

      // remove points above the window limit
      while(last != end)
      {
        if(last->time < cur->time)
          // vector is not sorted by time
          throw NormalizeException();
        if(last->time >= cur->time + window)
          break;
        ++cnt;
        total += (double) last->value;
      }

      assert(cnt > 0);
      normed.push_back(Point{cur->time, cur->value - (float) (total/cnt)});
    }

    assert(raw.size() == normed.size());
    return normed;
  }

  void Data::normalize(float range)
  { this->normed = sc::normalize(this->raw, range); }

  Data open(Model const & mdl)
  {
    Data data;
    if(boost::filesystem::exists(mdl.path))
    {
      if(ext7z::_isfile(mdl.path))
        data.raw = std::move(ext7z::_open(mdl.path, mdl.rate));
      else if(exttxt::_isfile(mdl.path))
        data.raw = std::move(ext7z::_open(mdl.path, mdl.rate));

      data.normalize(mdl.slidingrange);
    }
    return data;
  }
}
