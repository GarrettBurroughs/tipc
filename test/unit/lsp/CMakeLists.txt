add_executable(lsp_unit_tests)
target_sources(lsp_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/RpcTest.cpp)

target_include_directories(
  lsp_unit_tests
  PRIVATE ${CMAKE_SOURCE_DIR}/src/lsp)

target_link_libraries(
  lsp_unit_tests
  PRIVATE lsp
          test_helpers
          coverage_config
          Catch2::Catch2WithMain
          nlohmann_json::nlohmann_json)
