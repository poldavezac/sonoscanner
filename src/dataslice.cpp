#include "gui.h"
namespace sc {
  float DataSlice::centerperiod(Model const & model)
  {
      // * 1/model.refreshrate:
      //    time between refresh calls (user time)
      // * model.windowrate:
      //    number of *new* points displayed in 1 second (user time)
      // * model.windowrate/model.refreshrate:
      //    number of *new* points displayed between refresh
      // * 1/model.rate:
      //    time between 2 data points (data time)
      // * 1.f / model.rate * (model.windowrate / model.refreshrate):
      //    time between two refresh (data time)
      return 1.f /model.rate * (model.windowrate / model.refreshrate);
  }

  bool DataSlice::operator==(DataSlice const & right) const
  {
    return this->begin == right.begin && this->end == right.end && this->center == right.center;
  }

  DataSlice DataSlice::refresh(Model const & model, Data const & data) const
  {
    if(this->end >= data.normed.size())
      return *this;

    // we have not yet reached the end of the data
    auto next = DataSlice{
      this->begin, this->end, this->center + DataSlice::centerperiod(model)
    };
    auto hrng = model.windowrange * .5;

    // move lower end
    float first = next.center - hrng;
    for(size_t iend = data.normed.size(); next.begin < iend; ++next.begin)
      if(data.normed[next.begin].time >= first)
        break;

    // move upper end
    float last = next.center + hrng;
    for(size_t iend = data.normed.size(); next.end < iend; ++next.end)
      if(data.normed[next.end].time > last)
        break;

    if(data.normed[next.end].time <= last) // this happens at the very last refresh
      ++next.end;
    return next;
  }
}
