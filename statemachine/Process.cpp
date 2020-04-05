#include "Process.hpp"

#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/selection/SelectionStack.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/EntitySerialization.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <wobjectimpl.h>

W_OBJECT_IMPL(statemachine::Model)
namespace statemachine
{

Model::Model(
      const TimeVal& duration,
      const Id<Process::ProcessModel>& id,
      const score::DocumentContext& ctx,
      QObject* parent)
    : Process::ProcessModel{duration, id, "statemachineProcess", parent}
    , m_context{ctx}
{
  metadata().setInstanceName(*this);
}

Model::~Model()
{
  try
  {
    m_context.selectionStack.clear();
  }
  catch (...)
  {
    // Sometimes the scenario isn't in the hierarchy, e.G. in
    // ScenarioPasteElements
  }
  intervals.clear();
  states.clear();
  events.clear();
  timeSyncs.clear();

  identified_object_destroying(this);
}



}
template <>
void DataStreamReader::read(const statemachine::Model& proc)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(statemachine::Model& proc)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const statemachine::Model& proc)
{
}

template <>
void JSONObjectWriter::write(statemachine::Model& proc)
{
}



template <>
void DataStreamReader::read<statemachine::Edge>(const statemachine::Edge& p)
{
  m_stream << p.m_source << p.m_sink;
}
template <>
void DataStreamWriter::write<statemachine::Edge>(statemachine::Edge& p)
{
  m_stream >> p.m_source >> p.m_sink;
}
template <>
void JSONObjectReader::read<statemachine::Edge>(const statemachine::Edge& p)
{
  obj["Source"] = toJsonValue(p.m_source);
  obj["Sink"] = toJsonValue(p.m_sink);
}
template <>
 void JSONObjectWriter::write<statemachine::Edge>(statemachine::Edge& p)
{
   p.m_source = fromJsonValue<Id<Scenario::IntervalModel>>(obj["Source"]);
   p.m_sink = fromJsonValue<Id<Scenario::IntervalModel>>(obj["Sink"]);
}

