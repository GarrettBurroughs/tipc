#include "LSPState.h"
#include "FrontEnd.h"
#include "GetVariableNode.h"
#include "ParseError.h"
#include "SemanticAnalysis.h"
#include "loguru.hpp"
#include "messages.h"
#include <optional>
#include <sstream>

std::optional<ParseError> LSPState::openDocument(std::string uri,
                                                 std::string text) {
  documents[uri] = text;
  std::stringstream stream(text);
  analysisResults[uri] = std::nullopt;

  // Right now, the lsp will work off of the most recently properly parsed
  // document when providing information If the parsing fails, it will be marked
  // in the updatedAST map, some operations should work on the last correct AST
  // while others should fail to return information if the ast is not up to date
  try {
    asts[uri] = FrontEnd::parse(stream);
    updatedAst[uri] = true;
  } catch (ParseError e) {
    updatedAst[uri] = false;
    return e;
  }
  return std::nullopt;
}

std::optional<ParseError> LSPState::updateDocument(std::string uri,
                                                   std::string text) {
  documents[uri] = text;
  std::stringstream stream(text);
  analysisResults[uri] = std::nullopt;
  try {
    asts[uri] = FrontEnd::parse(stream);
    updatedAst[uri] = true;
  } catch (ParseError e) {
    updatedAst[uri] = false;
    return e;
  }
  return std::nullopt;
}

std::string LSPState::getDocument(std::string uri) { return documents[uri]; }

std::vector<std::string> LSPState::splitLines(const std::string &content) {
  std::vector<std::string> lines;
  size_t start = 0, end;
  while ((end = content.find('\n', start)) != std::string::npos) {
    lines.push_back(content.substr(start, end - start));
    start = end + 1;
  }
  lines.push_back(content.substr(start));
  return lines;
}

// Returns most recent valid AST (if any)
std::optional<std::shared_ptr<ASTProgram>> LSPState::getAst(std::string uri) {
  if (asts.count(uri) != 0) {
    return asts[uri];
  }
  return std::nullopt;
}

// Returns current AST if document is valid
std::optional<std::shared_ptr<ASTProgram>>
LSPState::getUpdatedAst(std::string uri) {
  if (asts.count(uri) != 0 && updatedAst[uri]) {
    return asts[uri];
  }
  return std::nullopt;
}

std::pair<int, std::string>
LSPState::getWordAtCursor(const std::string &content, int line, int character) {
  std::vector<std::string> lines = splitLines(content);

  if (line < 0 || line >= static_cast<int>(lines.size())) {
    throw std::out_of_range("Line number out of range");
  }

  const std::string &targetLine = lines[line];

  if (character < 0 || character >= static_cast<int>(targetLine.size())) {
    throw std::out_of_range("Character index out of range");
  }

  int start = character;
  while (start > 0 && (std::isalnum(targetLine[start - 1]) ||
                       targetLine[start - 1] == '_')) {
    start--;
  }

  int end = character;
  while (end < static_cast<int>(targetLine.size()) &&
         (std::isalnum(targetLine[end]) || targetLine[end] == '_')) {
    end++;
  }

  return {start, targetLine.substr(start, end - start)};
}

HoverResponse LSPState::hover(HoverRequest request) {
  std::string uri = request.params.textDocument.uri;
  if (asts.count(uri) == 0) {
    return newHoverResponse(request.id, "");
  }
  std::string contents = documents[uri];
  Position position = request.params.position;
  auto [start, token] =
      getWordAtCursor(contents, position.line, position.character);

  auto ast = asts[request.params.textDocument.uri];

  GetVariableNode getVariableNode(token, position.line, start);
  ast->accept(&getVariableNode);

  std::stringstream ss;
  auto node = getVariableNode.getNode();
  if (node) {
    std::shared_ptr<SemanticAnalysis> analysisResult;
    if (analysisResults[uri]) {
      analysisResult = analysisResults[uri].value();
    } else {
      analysisResult = SemanticAnalysis::analyze(ast.get(), false);
      analysisResults[uri] = analysisResult;
    }

    auto symbolTable = analysisResult->getSymbolTable();
    if (getVariableNode.isFunction()) {
      auto fnType =
          analysisResult->getTypeResults()->getInferredType(node.value());
      std::stringstream typeName;
      typeName << token << ": " << *fnType;
      return newHoverResponse(request.id, typeName.str());
    } else {
      start = std::time(NULL);
      auto varNode = symbolTable->getLocal(token, node.value());
      auto varType = analysisResult->getTypeResults()->getInferredType(varNode);

      std::stringstream typeName;
      typeName << token << ": " << *varType;
      return newHoverResponse(request.id, typeName.str());
    }
  } else {
    return newHoverResponse(request.id, "");
  }
}
