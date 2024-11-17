add_library(lsp)

target_sources(
  lsp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/Lsp.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/Lsp.cpp)

target_include_directories(lsp PRIVATE)
target_link_libraries(lsp PRIVATE coverage_config)
