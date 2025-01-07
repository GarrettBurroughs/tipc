#pragma once

#include "ASTProgram.h"
#include "ParseError.h"
#include "SemanticAnalysis.h"
#include "messages/CompletionRequest.h"
#include "messages/HoverRequest.h"
#include <map>
#include <mutex>
#include <optional>
#include <string>

class LSPState {
private:
  std::map<std::string, std::string> documents;
  std::map<std::string, std::shared_ptr<ASTProgram>> asts;
  // Stores whether or not the current AST represents the most up to date
  // document
  std::map<std::string, bool> updatedAst;
  std::map<std::string, std::shared_ptr<SemanticAnalysis>> analysisResults;
  std::map<std::string, std::optional<std::shared_ptr<SemanticAnalysis>>>
      updatedAnalysisResults;
  std::map<std::string, int> versions;
  std::mutex lock;
  std::mutex &writeLock;
  std::vector<std::string> splitLines(const std::string &content);
  std::pair<int, std::string> getWordAtCursor(const std::string &content,
                                              int line, int character);

public:
  LSPState(std::mutex &writeLock) : documents(), writeLock(writeLock) {}

  std::optional<ParseError> openDocument(std::string uri, std::string text);
  std::optional<ParseError> updateDocument(std::string uri, std::string text,
                                           int version);
  std::string getDocument(std::string uri);
  std::optional<std::shared_ptr<ASTProgram>> getAst(std::string uri);
  std::optional<std::shared_ptr<ASTProgram>> getUpdatedAst(std::string uri);
  HoverResponse hover(HoverRequest request);
  CompletionResponse autocomplete(CompletionRequest request);
};
