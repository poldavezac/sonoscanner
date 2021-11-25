#include <thread>
#include "gui.h"

namespace sc {
  // gui interactions
  namespace {
    bool _update_line(Gui & gui)
    {
      auto & slice = gui.live.slice;
      auto next = slice.refresh(gui.live.model, gui.live.data);
      if(next == slice)
        return false;

      auto & normed = gui.live.data.normed;
      if(
          (slice.begin == slice.end)
          || (slice.begin > next.begin)
          || (slice.end > next.end)
      ) {
        // refresh completely
        gui.series->clear();
        for(auto i = next.begin; i < next.end; ++i)
          gui.series->append(normed[i].time, normed[i].value);
      }
      else
      {
        // update points
        gui.series->removePoints(0, next.begin - slice.begin);
        for(auto i = slice.end; i < next.end; ++i)
          gui.series->append(normed[i].time, normed[i].value);
      }
      slice = next;
      return true;
    }

    template <Qt::Orientation> qreal _axis_min(Gui const & gui);
    template <Qt::Orientation> qreal _axis_max(Gui const & gui);
    template <> qreal _axis_min<Qt::Horizontal>(Gui const & gui)
    { return gui.series->points().front().rx(); }
    template <> qreal _axis_max<Qt::Horizontal>(Gui const & gui)
    { return gui.series->points().back().rx(); }
    template <> qreal _axis_min<Qt::Vertical>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::max();
      for(auto pt: gui.series->points())
        val = std::min(val, pt.ry());
      return val;
    }
    template <> qreal _axis_max<Qt::Vertical>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::min();
      for(auto pt: gui.series->points())
        val = std::max(val, pt.ry());
      return val;
    }

    template <Qt::Orientation O>
    inline void _update_axis(Gui const & gui)
    {
      // expecting only one axis
      assert(gui.chart->axes(O).size() == 1lu);
      auto axis = gui.chart->axes(O).front();
      if(gui.series->points().size() == 0lu)
      {
        axis->setMin(0.f);
        axis->setMax(1.f);
      } else {
        auto front = _axis_min<O>(gui);
        auto back  = _axis_max<O>(gui);
        axis->setMin(front - (back-front) * gui.live.model.axispadding);
        axis->setMax(back  + (back-front) * gui.live.model.axispadding);
      }
    }

    void _update_chart(Gui & gui)
    {
      auto & model = gui.live.model;
      if(model.state == kRunning && _update_line(gui))
      {
        _update_axis<Qt::Horizontal>(gui);
        if(model.zoom == kAuto)
          _update_axis<Qt::Vertical>(gui);
        gui.chart->update();
      }
    }
  }

  void Gui::runchartthread()
  {
    while(this->live.model.state != kDisabled)
    {
      if(this->live.mutex.try_lock())
      {
        try{
          _update_chart(*this);
        }
        catch (...){
          this->live.mutex.unlock();
          std::throw_with_nested(std::runtime_error("Failed update"));
        }
      }
      std::this_thread::sleep_for(
          std::chrono::milliseconds(
            (int) std::round(1000.f/this->live.model.refreshrate)
          )
      );
    }
  }
}
