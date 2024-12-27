#pragma once

#include "TextDocument.h"
#include <optional>
#include <vector>

struct FormattingOptions {
  int tabSize;
  bool insertSpaces;
  std::optional<bool> trimTrailingWhitespace;
  std::optional<bool> insertFinalNewline;
  std::optional<bool> trimFinalNewlines;

  friend void to_json(nlohmann::json &j, const FormattingOptions &r) {
    j["tabSize"] = r.tabSize;
    j["insertSpaces"] = r.insertSpaces;
    if (r.trimTrailingWhitespace) {
      j["trimTrailingWhitespace"] = r.trimTrailingWhitespace.value();
    }

    if (r.trimFinalNewlines) {
      j["trimFinalNewlines"] = r.trimFinalNewlines.value();
    }

    if (r.insertFinalNewline) {
      j["insertFinalNewline"] = r.insertFinalNewline.value();
    }
  }

  friend void from_json(const nlohmann::json &j, FormattingOptions &r) {
    if (j.contains("trimTrailingWhitespace")) {
      r.trimTrailingWhitespace = j.at("trimTrailingWhitespace").get<bool>();
    } else {
      r.trimTrailingWhitespace = std::nullopt;
    }

    if (j.contains("insertFinalNewline")) {
      r.insertFinalNewline = j.at("insertFinalNewline").get<bool>();
    } else {
      r.insertFinalNewline = std::nullopt;
    }

    if (j.contains("trimFinalNewlines")) {
      r.trimFinalNewlines = j.at("trimFinalNewlines").get<bool>();
    } else {
      r.trimFinalNewlines = std::nullopt;
    }
    r.tabSize = j.at("tabSize").get<int>();
    r.insertSpaces = j.at("insertSpaces").get<bool>();
  }
};

struct DocumentFormattingParams {

  TextDocumentIdentfier textDocument;

  FormattingOptions options;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(DocumentFormattingParams, textDocument,
                                 options)
};

struct DocumentFormattingRequest {
  int id;
  std::string jsonrpc;

  DocumentFormattingParams params;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(DocumentFormattingRequest, id, jsonrpc, params)
};

struct DocumentFormattingResponse {
  std::string jsonrpc;
  std::optional<int> id;
  std::optional<std::vector<TextEdit>> result;

  friend void to_json(nlohmann::json &j, const DocumentFormattingResponse &r) {
    j["jsonrpc"] = r.jsonrpc;

    if (r.result) {
      j["result"] = r.result.value();
    }

    if (r.id) {
      j["id"] = r.id.value();
    }
  }

  friend void from_json(const nlohmann::json &j,
                        DocumentFormattingResponse &r) {
    if (j.contains("id")) {
      r.id = j.at("id").get<int>();
    } else {
      r.id = std::nullopt;
    }

    if (j.contains("result")) {
      r.result = j.at("result").get<std::vector<TextEdit>>();
    } else {
      r.id = std::nullopt;
    }

    r.jsonrpc = j.at("jsonrpc").get<std::string>();
  }
};
