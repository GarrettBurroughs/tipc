add_library(lsp)

target_sources(
  lsp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/LSP.h
               ${CMAKE_CURRENT_SOURCE_DIR}/LSP.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.h
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.cpp
           )

target_include_directories(lsp PRIVATE)
target_link_libraries(lsp PRIVATE coverage_config nlohmann_json::nlohmann_json)
