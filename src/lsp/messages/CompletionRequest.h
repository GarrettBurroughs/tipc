#pragma once

#include "TextDocument.h"
#include "nlohmann/detail/macro_scope.hpp"
#include <optional>
#include <vector>

enum class CompletionItemKind {
  Text = 1,
  Method = 2,
  Function = 3,
  Constructor = 4,
  Field = 5,
  Variable = 6,
  Class = 7,
  Interface = 8,
  Module = 9,
  Property = 10,
  Unit = 11,
  Value = 12,
  Enum = 13,
  Keyword = 14,
  Snippet = 15,
  Color = 16,
  File = 17,
  Reference = 18,
  Folder = 19,
  EnumMember = 20,
  Constant = 21,
  Struct = 22,
  Event = 23,
  Operator = 24,
  TypeParameter = 25,
};

enum class InsertTextFormat {
  PlainText = 1,
  Snippet = 2,
};

struct CompletionItem {
  std::string label;
  std::string detail;
  std::string documentation;
  std::string insertText;
  CompletionItemKind kind;
  InsertTextFormat insertTextFormat;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(CompletionItem, label, detail, documentation,
                                 kind, insertText, insertTextFormat)
};

struct CompletionParams : TextDocumentPositionParams {};

struct CompletionRequest {
  int id;
  std::string jsonrpc;
  CompletionParams params;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(CompletionRequest, id, jsonrpc, params)
};

struct CompletionResponse {
  std::string jsonrpc;
  std::optional<int> id;
  std::vector<CompletionItem> result;

  friend void to_json(nlohmann::json &j, const CompletionResponse &r) {
    j["jsonrpc"] = r.jsonrpc;
    j["result"] = r.result;
    if (r.id) {
      j["id"] = r.id.value();
    }
  }

  friend void from_json(const nlohmann::json &j, CompletionResponse &r) {
    if (j.contains("id")) {
      r.id = j.at("id").get<int>();
    } else {
      r.id = std::nullopt;
    }
    r.jsonrpc = j.at("jsonrpc").get<std::string>();
    r.result = j.at("result").get<std::vector<CompletionItem>>();
  }
};
