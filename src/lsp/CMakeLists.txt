add_library(lsp)

target_sources(
  lsp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/LSP.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.h
               ${CMAKE_CURRENT_SOURCE_DIR}/RPC.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/LSPState.h
               ${CMAKE_CURRENT_SOURCE_DIR}/LSPState.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/InitializeRequest.h
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/DidChangeTextDocumentNotification.h
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/DidOpenTextDocumentNotification.h
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/TextDocument.h
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/messages.h
               ${CMAKE_CURRENT_SOURCE_DIR}/messages/messages.cpp
           )

target_include_directories(
    lsp 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/messages
            ${CMAKE_SOURCE_DIR}/src/frontend
            ${CMAKE_SOURCE_DIR}/src/semantic
            ${CMAKE_SOURCE_DIR}/src/semantic/symboltable
            ${CMAKE_SOURCE_DIR}/src/semantic/types
            ${CMAKE_SOURCE_DIR}/src/semantic/types/concrete
            ${CMAKE_SOURCE_DIR}/src/semantic/types/constraints
            ${CMAKE_SOURCE_DIR}/src/semantic/types/solver
            ${CMAKE_SOURCE_DIR}/src/semantic/cfa
            ${CMAKE_SOURCE_DIR}/src/semantic/weeding
            ${CMAKE_SOURCE_DIR}/src/frontend/ast
            ${CMAKE_SOURCE_DIR}/src/frontend/ast/treetypes
            ${CMAKE_SOURCE_DIR}/src/error
    )
            

target_link_libraries(lsp PRIVATE error
          coverage_config
          loguru
          nlohmann_json::nlohmann_json)
