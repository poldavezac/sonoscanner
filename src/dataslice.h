#pragma once
#include "model.h"
#include "data.h"
namespace sc {
  // all information relative to the current window of data
  struct DataSlice
  {
    size_t begin  = 0lu;
    size_t end    = 0lu;
    float  center = 0.f;

    bool operator==(DataSlice const & right) const;

    // update for the next refresh
    DataSlice refresh(Model const &, Data const &) const;

    // In units of Element::time, the duration between
    // two refresh calls. This is function of the
    // data rate and the window and refresh rate.
    static float centerperiod(Model const & model);
  };
}
