add_library(lsp)

target_sources(
  lsp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/Lsp.h
               ${CMAKE_CURRENT_SOURCE_DIR}/Lsp.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/Rpc.h
               ${CMAKE_CURRENT_SOURCE_DIR}/Rpc.cpp
           )

target_include_directories(lsp PRIVATE)
target_link_libraries(lsp PRIVATE coverage_config nlohmann_json::nlohmann_json)
