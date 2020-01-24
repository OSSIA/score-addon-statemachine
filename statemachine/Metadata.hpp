#pragma once
#include <Process/ProcessMetadata.hpp>

namespace statemachine
{
class Model;
}

PROCESS_METADATA(
    , statemachine::Model, "4d9fb8f0-5133-4749-8514-94d33080e6ca",
    "statemachine",                                   // Internal name
    "statemachine",                                   // Pretty name
    Process::ProcessCategory::Other,              // Category
    "Other",                                      // Category
    "Description",                                // Description
    "Author",                                     // Author
    (QStringList{"Put", "Your", "Tags", "Here"}), // Tags
    {},                                           // Inputs
    {},                                           // Outputs
    Process::ProcessFlags::SupportsAll            // Flags
)
