#pragma once

#include "nlohmann/detail/macro_scope.hpp"
#include <nlohmann/json.hpp>
#include <string>

struct TextDocumentItem {
  std::string uri;
  std::string languageId;
  int version;
  std::string text;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextDocumentItem, uri, languageId, version,
                                 text)
};

struct TextDocumentIdentfier {
  std::string uri;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextDocumentIdentfier, uri)
};

struct VersionedTextDocumentIdentfier {
  std::string uri;
  int version;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(VersionedTextDocumentIdentfier, uri, version)
};

struct Position {
  int line;
  int character;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Position, line, character)
};

struct TextDocumentPositionParams {
  TextDocumentIdentfier textDocument;
  Position position;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextDocumentPositionParams, textDocument,
                                 position)
};

struct Range {
  Position start;
  Position end;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Range, start, end)
};

struct TextEdit {
  Range range;
  std::string newText;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextEdit, range, newText)
};
