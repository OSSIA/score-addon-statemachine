#pragma once
#include <Process/GenericProcessFactory.hpp>

#include <statemachine/Presenter.hpp>
#include <statemachine/Process.hpp>
#include <statemachine/View.hpp>

namespace statemachine
{
using LayerFactory = Process::LayerFactory_T<
    statemachine::Model, statemachine::Presenter, statemachine::View>;
}
