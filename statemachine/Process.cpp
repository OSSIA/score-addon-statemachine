#include "Process.hpp"

#include <wobjectimpl.h>

W_OBJECT_IMPL(statemachine::Model)
namespace statemachine
{

Model::Model(
    const TimeVal& duration, const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{duration, id, "statemachineProcess", parent}
{
  metadata().setInstanceName(*this);
}

Model::~Model()
{
}

QString Model::prettyName() const noexcept
{
  return tr("statemachine Process");
}

void Model::startExecution()
{
}

void Model::stopExecution()
{
}

void Model::reset()
{
}

void Model::setDurationAndScale(const TimeVal& newDuration) noexcept
{
}

void Model::setDurationAndGrow(const TimeVal& newDuration) noexcept
{
}

void Model::setDurationAndShrink(const TimeVal& newDuration) noexcept
{
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
