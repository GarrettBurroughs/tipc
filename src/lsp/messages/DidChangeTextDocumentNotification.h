#pragma once

#include "TextDocument.h"
#include <vector>


struct TextDocumentContentChangeEvent {
    std::string text;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextDocumentContentChangeEvent, text);
};

struct DidChangeTextDocumentParams {
    VersionedTextDocumentIdentfier textDocument;
    std::vector<TextDocumentContentChangeEvent> contentChanges;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DidChangeTextDocumentParams, textDocument, contentChanges);
};

struct DidChangeTextDocumentNotification {
    std::string jsonrpc;
    std::string method;
    DidChangeTextDocumentParams params;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DidChangeTextDocumentNotification, jsonrpc, method, params);
};
