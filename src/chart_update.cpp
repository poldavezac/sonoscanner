#include <thread>
#include "gui.h"

namespace sc {
  // gui interactions
  namespace {
    bool _update_line(Gui & gui, bool donext)
    {
      auto & slice = gui.live.slice;
      if(slice.begin == slice.end)
        donext = true; // happens when starting from 0

      auto next = donext ? slice.refresh(gui.live.model, gui.live.data) : slice;
      if(next == slice && gui.series->points().size() > 0lu)
        return false;

      auto & normed = gui.live.data.normed;
      if(
          gui.series->points().size() == 0lu
          || (slice.begin == slice.end)
          || (slice.begin > next.begin)
          || (slice.end > next.end)
      ) {
        // refresh completely
        gui.series->clear();
        qDebug("Displaying from %lu to %lu", 0lu, next.end);
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

    template <Qt::Orientation, bool Z> qreal _axis_min(Gui const & gui);
    template <Qt::Orientation, bool Z> qreal _axis_max(Gui const & gui);
    template <> qreal _axis_min<Qt::Horizontal, true>(Gui const & gui)
    { return gui.series->points().front().rx(); }
    template <> qreal _axis_max<Qt::Horizontal, true>(Gui const & gui)
    { return gui.series->points().back().rx(); }
    template <> qreal _axis_min<Qt::Vertical, true>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::max();
      for(auto pt: gui.series->points())
        val = std::min(val, pt.ry());
      return val;
    }
    template <> qreal _axis_max<Qt::Vertical, true>(Gui const & gui)
    {
      auto val = std::numeric_limits<qreal>::min();
      for(auto pt: gui.series->points())
        val = std::max(val, pt.ry());
      return val;
    }
    template <> qreal _axis_min<Qt::Vertical, false>(Gui const & gui)
    {
      auto txt = gui.ymin->text().toStdString();
      try { return std::stof(txt); } catch(...) {}
      return _axis_min<Qt::Vertical, false>(gui);
    }
    template <> qreal _axis_max<Qt::Vertical, false>(Gui const & gui)
    {
      auto txt = gui.ymax->text().toStdString();
      try { return std::stof(txt); } catch(...) {}
      return _axis_max<Qt::Vertical, false>(gui);
    }

    inline void _axis_range(Gui const & gui, qreal &front, qreal &back)
    {
      if(gui.series->points().size() == 0lu)
        return;
      auto rng = (back-front) * gui.live.model.axispadding;
      front -= rng;
      back  += rng;
    }

    template <Qt::Orientation O, bool Z = true>
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
        auto front = _axis_min<O, Z>(gui);
        auto back  = _axis_max<O, Z>(gui);
        if(Z)
          // Z = false is for fixed range: don't adapt it
          _axis_range(gui, front, back);
        axis->setMin(front);
        axis->setMax(back);
      }
    }

    void _update_yaxis_lineedit(Gui & gui)
    {
      auto front = _axis_min<Qt::Vertical, true>(gui);
      auto back  = _axis_max<Qt::Vertical, true>(gui);
      _axis_range(gui, front, back);
      if(!gui.ymin->hasFocus())
      {
        auto value = std::to_string(front);
        gui.ymin->setText(value.data());
      }
      if(!gui.ymin->hasFocus())
      {
        auto value = std::to_string(back);
        gui.ymax->setText(value.data());
      }
    }

    void _update_chart(Gui & gui, bool donext)
    {
      auto & model = gui.live.model;
      if(model.state != kDisabled && _update_line(gui, donext))
      {
        _update_axis<Qt::Horizontal, true>(gui);
        if(model.zoom == kAuto)
          _update_axis<Qt::Vertical, true>(gui);
        else
          _update_axis<Qt::Vertical, false>(gui);
        gui.chart->update();
      }

      if((model.state == kRefresh || model.state == kRunning) && model.zoom == kAuto)
        _update_yaxis_lineedit(gui);
    }

    bool _doupdate(Gui & gui, bool donext)
    {
        if(gui.live.mutex.try_lock())
        {
          try { _update_chart(gui, donext); }
          catch (...){
            std::throw_with_nested(std::runtime_error("Failed update"));
          }
          gui.live.mutex.unlock();
          return true;
        }
        return false;
    }
  }

  void Gui::runchartthread()
  {
    while(this->live.model.state != kDisabled)
    {
      if(this->live.model.state == kRefresh)
      {
        // TODO: setup a Qt signal to trigger this automatically
        this->series->clear();
        if(_doupdate(*this, false))
          this->live.model.state = kStopped;
      }
      if(this->live.model.state == kRunning)
        _doupdate(*this, true);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(
            (int) std::round(1000.f/this->live.model.refreshrate)
          )
      );
    }
  }
}
