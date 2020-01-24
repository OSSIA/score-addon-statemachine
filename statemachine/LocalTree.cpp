#include "LocalTree.hpp"

#include <LocalTree/Property.hpp>
#include <statemachine/Process.hpp>

namespace statemachine
{
LocalTreeProcessComponent::LocalTreeProcessComponent(
    const Id<score::Component>& id, ossia::net::node_base& parent,
    statemachine::Model& proc, const score::DocumentContext& sys, QObject* parent_obj)
    : LocalTree::ProcessComponent_T<statemachine::Model>{
          parent, proc, sys, id, "statemachineComponent", parent_obj}
{
}

LocalTreeProcessComponent::~LocalTreeProcessComponent()
{
}
}
