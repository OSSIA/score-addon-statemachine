#pragma once
#include <score/command/Command.hpp>
#include <score/tools/Unused.hpp>
#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <score/model/path/Path.hpp>
#include <Scenario/Tools/dataStructures.hpp>
#include <statemachine/CommandFactory.hpp>

namespace Scenario
{
class IntervalModel;
class StateModel;
class EventModel;
}
namespace statemachine
{
class Model;
enum EventPosition {
  BeforeInterval,
  AfterInterval
};

std::pair<const Scenario::IntervalModel&, EventPosition> getInterval(
    const Scenario::EventModel& ev,
    const Model& m);
class MoveEvent final : public score::Command
{
 SCORE_COMMAND_DECL(
     CommandFactoryName(),
     MoveEvent,
     "Move an event")
public:
  MoveEvent(
      const Model& scenar,
      const Id<Scenario::EventModel>& event,
      const TimeVal& date,
      double y,
      ExpandMode mode,
      LockMode);

  MoveEvent(
      const Model& scenar,
      const Id<Scenario::EventModel>& event,
      const TimeVal& date,
      double y,
      ExpandMode mode,
      LockMode lm,
      Id<Scenario::StateModel>);

  void undo(const score::DocumentContext& ctx) const override;

  void redo(const score::DocumentContext& ctx) const override;

  void
  update(unused_t, unused_t, const TimeVal& date, double, ExpandMode, LockMode);
  void update(
      unused_t,
      unused_t,
      const TimeVal& date,
      double,
      ExpandMode,
      LockMode,
      unused_t);

  const Path<Model>& path() const;

private:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

  template <typename ScaleFun>
  void updateDuration(
      Scenario::IntervalModel& interval,
      Model& sm,
      const TimeVal& newDuration,
      ScaleFun&& scaleMethod) const;


  Path<Model> m_path;
  Id<Scenario::IntervalModel> m_interval;

  TimeVal m_oldDate{};
  TimeVal m_newDate{};

  ExpandMode m_mode{ExpandMode::Scale};
  EventPosition m_pos;

  Scenario::IntervalSaveData m_saveData;
};


}
