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

  // all information relative to the GUI configuration
  struct Theme
  {
    size_t      width  = 400;
    size_t      height = 400;
    std::string title  = "Simple line chart example";
    std::string starttitle = "Start";
    std::string stoptitle  = "Stop";
    std::string quittitle  = "Quit";
  };

  int run(int argc, char **argv, Model & model, Data & data);
}
