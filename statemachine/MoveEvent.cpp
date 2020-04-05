#include <statemachine/MoveEvent.hpp>

#include <statemachine/Process.hpp>
#include <Scenario/Tools/dataStructures.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Process/TimeValueSerialization.hpp>
#include <score/model/path/PathSerialization.hpp>
namespace statemachine
{

template <typename ScaleFun>
void MoveEvent::updateDuration(
    Scenario::IntervalModel& interval,
    Model& sm,
    const TimeVal& newDuration,
    ScaleFun&& scaleMethod) const
{
  Scenario::endEvent(interval, sm).setDate(newDuration);
  Scenario::endTimeSync(interval, sm).setDate(newDuration);
  Scenario::IntervalDurations::Algorithms::changeAllDurations(interval, newDuration);
  for (auto& process : interval.processes)
  {
    scaleMethod(process, newDuration);
  }
}

MoveEvent::MoveEvent(const Model& scenar, const Id<Scenario::EventModel>& event, const TimeVal& date, double y, ExpandMode mode, LockMode)
  : m_path{scenar}, m_newDate{date}, m_mode{mode}
{
  auto [itv, pos] = getInterval(scenar.event(event), scenar);
  m_interval = itv.id();

  m_oldDate = itv.duration.defaultDuration();
  m_saveData = Scenario::IntervalSaveData{itv, true}; // TODO fix the "clear" under this
}

MoveEvent::MoveEvent(const Model& scenar, const Id<Scenario::EventModel>& event, const TimeVal& date, double y, ExpandMode mode, LockMode lm, Id<Scenario::StateModel>)
  : MoveEvent{scenar, event, date, y, mode, lm}
{
}

void MoveEvent::undo(const score::DocumentContext& ctx) const
{
  /*
    auto& scenar = m_path.find(ctx);

    updateDuration(
        scenar, m_oldDate, [&](Process::ProcessModel& p, const TimeVal& v) {
          // Nothing is needed since the processes will be replaced anyway.
        });

    // TODO do this only if we shrink.

    // Now we have to restore the state of each interval that might have been
    // modified
    // during this command.

    // 1. Clear the interval
    ClearInterval clearCmd{scenar.interval()};
    clearCmd.redo(ctx);

    // 2. Restore
    auto& interval = scenar.interval();
    m_saveData.reload(interval);
    */
}

void MoveEvent::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  auto& itv = scenar.interval(m_interval);
  updateDuration(itv, scenar, m_newDate,
                 [&](Process::ProcessModel& p, const TimeVal& v) {
    p.setParentDuration(m_mode, v);
  });
}

void MoveEvent::update(unused_t, unused_t, const TimeVal& date, double, ExpandMode, LockMode)
{
  m_newDate = date;
}

void MoveEvent::update(unused_t, unused_t, const TimeVal& date, double, ExpandMode, LockMode, unused_t)
{
  m_newDate = date;
}

const Path<Model>& MoveEvent::path() const { return m_path; }

void MoveEvent::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_oldDate << m_newDate << (int)m_mode << m_saveData;
}

void MoveEvent::deserializeImpl(DataStreamOutput& s)
{
  int mode;
  s >> m_path >> m_oldDate >> m_newDate >> mode >> m_saveData;
  m_mode = static_cast<ExpandMode>(mode);
}

std::pair<const Scenario::IntervalModel&, EventPosition> getInterval(const Scenario::EventModel& ev, const Model& m)
{
  for(auto& state : ev.states())
  {
    auto& st = m.state(state);
    if(auto itv = st.previousInterval())
    {
      return std::pair<const Scenario::IntervalModel&, EventPosition>{m.interval(*itv), AfterInterval};
    }
    if(auto itv = st.nextInterval())
    {
      return std::pair<const Scenario::IntervalModel&, EventPosition>{m.interval(*itv), BeforeInterval};
    }
  }
  SCORE_ABORT;
}

}
