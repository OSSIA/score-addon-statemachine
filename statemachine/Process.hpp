#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Process/Process.hpp>

#include <statemachine/Metadata.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>

namespace statemachine
{
class Cell
{
public:
  Id<Scenario::IntervalModel> interval;
  Id<Scenario::StateModel> startState;
  Id<Scenario::StateModel> endState;
  Id<Scenario::EventModel> startEvent;
  Id<Scenario::EventModel> endEvent;
  Id<Scenario::TimeSyncModel> startSync;
  Id<Scenario::TimeSyncModel> endSync;
};

class Edge : public IdentifiedObject<Edge>
{
  W_OBJECT(Edge)
  SCORE_SERIALIZE_FRIENDS
public:
  Selectable selection;

  Edge() = delete;
  ~Edge();
  Edge(const Edge&) = delete;
  Edge(Id<Edge> c, QObject* parent)
    : IdentifiedObject{c, QStringLiteral("Edge"), parent}
  {

  }

  template <typename T>
  Edge(T&& vis, QObject* parent)
    : IdentifiedObject{vis, parent}
  {
    vis.writeTo(*this);
  }

  void setSource(const Id<Scenario::IntervalModel>& source)
  {

  }
  void setSink(const Id<Scenario::IntervalModel>& sink)
  {

  }

  const Id<Scenario::IntervalModel>& source() const noexcept
  {
    return m_source;
  }

  const Id<Scenario::IntervalModel>& sink() const noexcept
  {
    return m_sink;
  }

  void sourceChanged(const Id<Scenario::IntervalModel>& s) W_SIGNAL(sourceChanged, s)
  void sinkChanged(const Id<Scenario::IntervalModel>& s) W_SIGNAL(sinkChanged, s)
private:
  W_PROPERTY(Id<Scenario::IntervalModel>, source READ source WRITE setSource NOTIFY sourceChanged)
  W_PROPERTY(Id<Scenario::IntervalModel>, sink READ sink WRITE setSink NOTIFY sinkChanged)

  Id<Scenario::IntervalModel> m_source;
  Id<Scenario::IntervalModel> m_sink;
};

class Model final
    : public Process::ProcessModel
    , public Scenario::ScenarioInterface
{
  SCORE_SERIALIZE_FRIENDS
  PROCESS_METADATA_IMPL(statemachine::Model)
  W_OBJECT(Model)

public:
  using IntervalModel = Scenario::IntervalModel;
  using EventModel = Scenario::EventModel;
  using StateModel = Scenario::StateModel;
  using TimeSyncModel = Scenario::TimeSyncModel;

  std::unique_ptr<Process::AudioInlet> inlet;
  std::unique_ptr<Process::AudioOutlet> outlet;

  Model(
      const TimeVal& duration, const Id<Process::ProcessModel>& id,
      const score::DocumentContext& ctx,
      QObject* parent);

  template <typename Impl>
  Model(Impl& vis, const score::DocumentContext& ctx, QObject* parent)
    : Process::ProcessModel{vis, parent}
    , m_context{ctx}
  {
    vis.writeTo(*this);
  }

  ~Model() override;
  using QObject::event;


  score::EntityMap<Scenario::IntervalModel> intervals;
  score::EntityMap<Scenario::EventModel> events;
  score::EntityMap<Scenario::TimeSyncModel> timeSyncs;
  score::EntityMap<Scenario::StateModel> states;
  score::EntityMap<Edge> edges;

  // Accessors
  score::IndirectContainer<IntervalModel> getIntervals() const final override
  {
    return intervals.map().as_indirect_vec();
  }

  score::IndirectContainer<StateModel> getStates() const final override
  {
    return states.map().as_indirect_vec();
  }

  score::IndirectContainer<EventModel> getEvents() const final override
  {
    return events.map().as_indirect_vec();
  }

  score::IndirectContainer<TimeSyncModel> getTimeSyncs() const final override
  {
    return timeSyncs.map().as_indirect_vec();
  }

  IntervalModel* findInterval(const Id<IntervalModel>& id) const final override
  {
    return ossia::ptr_find(intervals, id);
  }
  EventModel* findEvent(const Id<EventModel>& id) const final override
  {
    return ossia::ptr_find(events, id);
  }
  TimeSyncModel* findTimeSync(const Id<TimeSyncModel>& id) const final override
  {
    return ossia::ptr_find(timeSyncs, id);
  }
  StateModel* findState(const Id<StateModel>& id) const final override
  {
    return ossia::ptr_find(states, id);
  }

  IntervalModel&
  interval(const Id<IntervalModel>& intervalId) const final override
  {
    return intervals.at(intervalId);
  }
  EventModel& event(const Id<EventModel>& eventId) const final override
  {
    return events.at(eventId);
  }
  TimeSyncModel&
  timeSync(const Id<TimeSyncModel>& timeSyncId) const final override
  {
    return timeSyncs.at(timeSyncId);
  }
  StateModel& state(const Id<StateModel>& stId) const final override
  {
    return states.at(stId);
  }
private:
  const score::DocumentContext& m_context;

};

using ProcessFactory = Process::ProcessFactory_T<statemachine::Model>;
}
