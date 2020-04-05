#include "score_addon_statemachine.hpp"

#include <score/plugins/FactorySetup.hpp>

#include <statemachine/CommandFactory.hpp>
#include <statemachine/Executor.hpp>
#include <statemachine/Inspector.hpp>
#include <statemachine/Layer.hpp>
#include <statemachine/LocalTree.hpp>
#include <statemachine/Process.hpp>
#include <score_addon_statemachine_commands_files.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
namespace statemachine
{
class LayerFactory final : public Process::LayerFactory
{
public:
  LayerFactory(Scenario::EditionSettings& e)
    : m_editionSettings{e}
  {
  }

  Process::LayerPresenter* makeLayerPresenter(
      const Process::ProcessModel& lm,
      Process::LayerView* view,
      const Process::Context& context,
      QObject* parent) const override
  {
    if (auto vm = dynamic_cast<const Model*>(&lm))
    {
      auto pres
          = new Presenter{m_editionSettings, *vm, view, context, parent};
      return pres;
    }
    return nullptr;
  }

  Process::LayerView* makeLayerView(
      const Process::ProcessModel& viewmodel,
      const Process::Context& context,
      QGraphicsItem* parent) const override
  {
      return new View{parent};
  }

  bool matches(const UuidKey<Process::ProcessModel>& p) const override
  {
    return p == Metadata<ConcreteKey_k, statemachine::Model>::get();
  }

  UuidKey<Process::ProcessModel> concreteKey() const noexcept override
  {
    return Metadata<ConcreteKey_k, statemachine::Model>::get();
  }

private:
  Scenario::EditionSettings& m_editionSettings;
};

}

template <>
struct FactoryBuilder<
    score::GUIApplicationContext,
    statemachine::LayerFactory>
{
  static auto make(const score::GUIApplicationContext& ctx)
  {
    using namespace statemachine;
    auto& appPlugin = ctx.guiApplicationPlugin<Scenario::ScenarioApplicationPlugin>();
    return std::make_unique<LayerFactory>(appPlugin.editionSettings());
  }
};
score_addon_statemachine::score_addon_statemachine()
{
}

score_addon_statemachine::~score_addon_statemachine()
{
}

std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_statemachine::factories(
    const score::ApplicationContext& ctx, const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Process::ProcessModelFactory, statemachine::ProcessFactory>,
      FW<Process::InspectorWidgetDelegateFactory, statemachine::InspectorFactory>,
      FW<Execution::ProcessComponentFactory,
         statemachine::ProcessExecutorComponentFactory>,
      FW<LocalTree::ProcessComponentFactory,
         statemachine::LocalTreeProcessComponentFactory>>(ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
score_addon_statemachine::make_commands()
{
  using namespace statemachine;
  std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
      CommandFactoryName(), CommandGeneratorMap{}};

  ossia::for_each_type<
#include <score_addon_statemachine_commands.hpp>
      >(score::commands::FactoryInserter{cmds.second});

  return cmds;
}
std::vector<std::unique_ptr<score::InterfaceBase>>
score_addon_statemachine::guiFactories(
    const score::GUIApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  using namespace statemachine;
  return instantiate_factories<
      score::GUIApplicationContext,
      FW<Process::LayerFactory, LayerFactory>>(ctx, key);
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_statemachine)
