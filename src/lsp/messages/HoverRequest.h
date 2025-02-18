#pragma once

#include "TextDocument.h"
#include <optional>

struct HoverParams {
  TextDocumentIdentfier textDocument;
  Position position;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(HoverParams, textDocument, position)
};

struct CompletionResult {
  std::string contents;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(CompletionResult, contents)
};

struct CompletionRequest {
  int id;
  std::string jsonrpc;

  HoverParams params;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(CompletionRequest, id, jsonrpc, params)
};

struct CompletionResponse {
  std::string jsonrpc;
  std::optional<int> id;
  CompletionResult result;

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
    r.result = j.at("result").get<CompletionResult>();
  }
};
