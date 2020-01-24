#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>

#include <statemachine/Process.hpp>

namespace statemachine
{
class InspectorWidget final
    : public Process::InspectorWidgetDelegate_T<statemachine::Model>
{
public:
  explicit InspectorWidget(
      const statemachine::Model& object, const score::DocumentContext& context,
      QWidget* parent);
  ~InspectorWidget() override;

private:
  CommandDispatcher<> m_dispatcher;
};

class InspectorFactory final
    : public Process::InspectorWidgetDelegateFactory_T<Model, InspectorWidget>
{
  SCORE_CONCRETE("e3a053ef-e486-4dc4-8697-92cf24e597b9")
};
}
