#include "score_addon_statemachine.hpp"

#include <score/plugins/FactorySetup.hpp>

#include <statemachine/CommandFactory.hpp>
#include <statemachine/Executor.hpp>
#include <statemachine/Inspector.hpp>
#include <statemachine/Layer.hpp>
#include <statemachine/LocalTree.hpp>
#include <statemachine/Process.hpp>
#include <score_addon_statemachine_commands_files.hpp>

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
      FW<Process::LayerFactory, statemachine::LayerFactory>,
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

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_statemachine)
