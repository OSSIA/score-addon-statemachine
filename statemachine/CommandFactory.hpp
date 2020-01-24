#pragma once
#include <score/command/Command.hpp>

namespace statemachine
{
inline const CommandGroupKey& CommandFactoryName()
{
  static const CommandGroupKey key{"statemachine"};
  return key;
}
}
