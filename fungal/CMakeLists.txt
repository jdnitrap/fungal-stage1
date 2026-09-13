cmake_minimum_required(VERSION 3.14)
project(Fungal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

enable_testing()

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTEST gtest)
        pkg_check_modules(GTEST_MAIN gtest_main)
    endif()
endif()

set(FOUNDATION_SOURCES
    src/hardware.cpp
    src/substrate.cpp
    src/introspection.cpp
    src/autonomous_node.cpp
    src/self_model.cpp
)

set(SAFETY_SOURCES
    src/constraints.cpp
    src/alignment.cpp
    src/monitor.cpp
)

set(LEARNING_SOURCES
    src/learning.cpp
    src/self_improvement.cpp
    src/neural_integration.cpp
)

set(CORE_LOOP_SOURCES
    src/core/hardware.cpp
    src/core/hardware_aware_scheduler.cpp
    src/core/self_model.cpp
    src/core/strategy.cpp
    src/core/energy_budget.cpp
    src/core/stage1_state.cpp
    src/core/control_loop.cpp
    src/core/test_oracle.cpp
    src/core/real_oracle.cpp
    src/core/external_oracle.cpp
)
set(LAYER1_SOURCES
    src/layer1/tokenizer.cpp
    src/layer1/analyzer.cpp
    src/layer1/dictionary.cpp
    src/layer1/verifier.cpp
    src/layer1/builder.cpp
    src/layer1/layer1.cpp
)

set(LAYER2_CORE_SOURCES
    src/layer2/core/node.cpp
    src/layer2/core/substrate.cpp
    src/layer2/core/energy.cpp
)

set(LAYER2_PHASES_SOURCES
    src/layer2/phases/phase1_claim_decay.cpp
    src/layer2/phases/phase2_energy_transfer.cpp
    src/layer2/phases/phase3_quarantine.cpp
    src/layer2/phases/phase4_removal.cpp
    src/layer2/phases/phase5_environmental_shocks.cpp
    src/layer2/phases/phase6_asymmetric_trust.cpp
    src/layer2/phases/phase7_specialization.cpp
    src/layer2/phases/phase8_substrate_learning.cpp
    src/layer2/phases/phase9_claim_refinement.cpp
    src/layer2/phases/phase10_multi_network_learning.cpp
    src/layer2/phases/phase11_measurement_uncertainty.cpp
    src/layer2/phases/phase12_hardware_constraints.cpp
)

set(LAYER2_VOTING_SOURCES
    src/layer2/voting/voting.cpp
)

set(BRIDGE_SOURCES
    src/bridge/bridge.cpp
)

set(LAYER2_SOURCES
    src/layer2/layer2.cpp
    ${LAYER2_CORE_SOURCES}
    ${LAYER2_PHASES_SOURCES}
    ${LAYER2_VOTING_SOURCES}
)

set(ALL_SOURCES
    ${FOUNDATION_SOURCES}
    ${SAFETY_SOURCES}
    ${LEARNING_SOURCES}
    src/fungal_system.cpp
    ${CORE_LOOP_SOURCES}
    ${LAYER1_SOURCES}
    ${LAYER2_SOURCES}
    ${BRIDGE_SOURCES}
)

add_library(fungal_lib ${ALL_SOURCES})
target_include_directories(fungal_lib PUBLIC include)
target_link_libraries(fungal_lib PUBLIC nlohmann_json::nlohmann_json)

add_executable(fungal src/main.cpp)
target_link_libraries(fungal fungal_lib)

add_executable(fungal_tui src/tui_app.cpp)
target_link_libraries(fungal_tui fungal_lib)

add_executable(fungal_sim src/simulation_mode.cpp)
target_link_libraries(fungal_sim fungal_lib)

add_executable(fungal_loop_demo src/core/control_loop_demo.cpp)
target_link_libraries(fungal_loop_demo fungal_lib)

add_executable(fungal_stage1_demo src/core/control_loop_stage1_demo.cpp)
target_link_libraries(fungal_stage1_demo fungal_lib)

add_executable(fungal_loop_halt_test src/core/control_loop_halt_test.cpp)
target_link_libraries(fungal_loop_halt_test fungal_lib)

add_executable(fungal_loop_real_demo src/core/control_loop_real_demo.cpp)
target_link_libraries(fungal_loop_real_demo fungal_lib)

add_executable(real_oracle_validator src/core/real_oracle_validator.cpp)
target_link_libraries(real_oracle_validator fungal_lib)

add_executable(fungal_loop_external_demo src/core/control_loop_external_demo.cpp)
target_link_libraries(fungal_loop_external_demo fungal_lib)

if(GTest_FOUND OR GTEST_FOUND)
    if(GTEST_FOUND)
        include_directories(${GTEST_INCLUDE_DIRS})
    endif()
    add_executable(test_tokenizer tests/unit/test_tokenizer.cpp)
    target_link_libraries(test_tokenizer fungal_lib GTest::gtest_main)
    add_test(NAME test_tokenizer COMMAND test_tokenizer LABELS "unit")

    add_executable(test_analyzer tests/unit/test_analyzer.cpp)
    target_link_libraries(test_analyzer fungal_lib GTest::gtest_main)
    add_test(NAME test_analyzer COMMAND test_analyzer LABELS "unit")

    add_executable(test_phases tests/unit/test_phases.cpp)
    target_link_libraries(test_phases fungal_lib GTest::gtest_main)
    add_test(NAME test_phases COMMAND test_phases LABELS "unit")

    add_executable(test_system_integration tests/integration/test_system.cpp)
    target_link_libraries(test_system_integration fungal_lib GTest::gtest_main)
    add_test(NAME test_system_integration COMMAND test_system_integration LABELS "integration")

    add_executable(test_stress tests/system/test_stress.cpp)
    target_link_libraries(test_stress fungal_lib GTest::gtest_main)
    add_test(NAME test_stress COMMAND test_stress LABELS "system")

    add_executable(test_self_aware tests/system/test_self_aware_system.cpp)
    target_link_libraries(test_self_aware fungal_lib)
    add_test(NAME test_self_aware COMMAND test_self_aware LABELS "system")

    message(STATUS "GTest found - building tests")
else()
    message(WARNING "GTest not found - tests skipped. Install libgtest-dev or set GTEST_INCLUDE_DIRS")
endif()
