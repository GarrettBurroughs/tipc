# compiler binary target
add_executable(tipc)
target_sources(tipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tipc.cpp)

# lsp binary target
add_executable(tiplsp)
target_sources(tiplsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lsp/LSP.cpp)

# Build the phase specific libraries
add_subdirectory(error)
add_subdirectory(frontend)
add_subdirectory(semantic)
add_subdirectory(codegen)
add_subdirectory(optimizer)
add_subdirectory(lsp)

target_link_libraries(
  tipc
  PRIVATE error
          frontend
          semantic
          codegen
          optimizer
          antlr4_static
          ${llvm_libs}
          coverage_config
          loguru
          nlohmann_json::nlohmann_json)

target_include_directories(
  tipc
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/error
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/ast
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/ast/treetypes
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/prettyprint
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/cfa
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/symboltable
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/concrete
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/constraints
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/solver
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/weeding
          ${CMAKE_CURRENT_SOURCE_DIR}/codegen
          ${CMAKE_CURRENT_SOURCE_DIR}/optimizer)

target_link_libraries(
  tiplsp
  PRIVATE error
          frontend
          semantic
          antlr4_static
          coverage_config
          loguru
          lsp
          nlohmann_json::nlohmann_json)

target_include_directories(
  tiplsp
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/error
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/ast
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/ast/treetypes
          ${CMAKE_CURRENT_SOURCE_DIR}/frontend/prettyprint
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/cfa
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/symboltable
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/concrete
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/constraints
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/types/solver
          ${CMAKE_CURRENT_SOURCE_DIR}/semantic/weeding
          ${CMAKE_CURRENT_SOURCE_DIR}/codegen
          ${CMAKE_CURRENT_SOURCE_DIR}/optimizer)
