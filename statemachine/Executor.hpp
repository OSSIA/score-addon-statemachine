#pragma once
#include <Process/Execution/ProcessComponent.hpp>

#include <ossia/dataflow/node_process.hpp>

namespace statemachine
{
class Model;
class ProcessExecutorComponent final
    : public Execution::ProcessComponent_T<
          statemachine::Model, ossia::node_process>
{
  COMPONENT_METADATA("aba8ce4d-4a2e-4aad-b149-5000bd778b8b")
public:
  ProcessExecutorComponent(
      Model& element, const Execution::Context& ctx,
      const Id<score::Component>& id, QObject* parent);
};

using ProcessExecutorComponentFactory
    = Execution::ProcessComponentFactory_T<ProcessExecutorComponent>;
}
