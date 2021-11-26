#pragma once
#include <string>
namespace sc {
  enum ZoomState { kAuto, kFixed };
  enum DisplayState { kRunning, kDisabled, kStopped };

  // Structure for holding all configrations
  // TODO: separate into multiple structures, depending on functionality (zoom, normalization, ...)
  // TODO: learn and use doxygen-styled comments
  struct Model {
    using data_t = int16_t;                 // the data size

    std::string  path;                      // path to the data
    float        rate         = 500;        // rate of data points (Hz)

    float        windowrange  = 6.;         // time range of the x-axis (s)
    float        windowrate   = 500.;       // rate of data flow in the window (Hz)
    float        refreshrate  = 20.;        // rate of updates in  range of the display (Hz)
    float        normalizationrange = 1.;         // normalization factor (s): sliding window size
    float        axispadding  = 0.05;       // data padding on each axis
    ZoomState    zoom         = kAuto;      // zoom mode
    DisplayState state        = kDisabled;  // state of the display: running, stopped, ...
  };
}
