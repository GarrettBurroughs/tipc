#include "TextDocument.h"

struct DidOpenTextDocucumentParams {
  TextDocumentItem textDocument;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(DidOpenTextDocucumentParams, textDocument);
};

struct DidOpenTextDocumentNotification {
  std::string jsonrpc;
  std::string method;
  DidOpenTextDocucumentParams params;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(DidOpenTextDocumentNotification, jsonrpc,
                                 method, params);
};
