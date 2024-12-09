add_library(lsp)

target_sources(
  lsp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/LSP.h
               ${CMAKE_CURRENT_SOURCE_DIR}/LSP.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.h
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/InitializeRequest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/InitializeRequest.h
           )

target_include_directories(
    lsp 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/messages
)

target_link_libraries(lsp PRIVATE coverage_config nlohmann_json::nlohmann_json)
