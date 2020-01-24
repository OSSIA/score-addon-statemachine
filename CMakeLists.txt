project(score_addon_statemachine LANGUAGES CXX)

# General initialization
score_common_setup()

# Source files
set(HDRS
    statemachine/Executor.hpp
    statemachine/Inspector.hpp
    statemachine/LocalTree.hpp
    statemachine/Metadata.hpp
    statemachine/Presenter.hpp
    statemachine/Process.hpp
    statemachine/View.hpp
    statemachine/Layer.hpp
    statemachine/CommandFactory.hpp

    score_addon_statemachine.hpp
)
set(SRCS
    statemachine/Executor.cpp
    statemachine/Inspector.cpp
    statemachine/LocalTree.cpp
    statemachine/Presenter.cpp
    statemachine/Process.cpp
    statemachine/View.cpp

    score_addon_statemachine.cpp
)


# Creation of the library
add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

# Code generation
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

# Link
target_link_libraries(${PROJECT_NAME} PUBLIC score_plugin_engine)

# Target-specific options
setup_score_plugin(${PROJECT_NAME})
