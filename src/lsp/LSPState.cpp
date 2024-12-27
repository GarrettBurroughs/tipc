#include "LSPState.h"
#include "FrontEnd.h"
#include "GetVariableNode.h"
#include "SemanticAnalysis.h"
#include "loguru.hpp"
#include "messages.h"
#include <sstream>

void LSPState::openDocument(std::string uri, std::string text) {
  documents[uri] = text;
}

void LSPState::updateDocument(std::string uri, std::string text) {
  documents[uri] = text;
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
  std::string contents = documents[request.params.textDocument.uri];
  Position position = request.params.position;
  auto [start, token] =
      getWordAtCursor(contents, position.line, position.character);

  std::stringstream inputStream(contents);
  std::shared_ptr<ASTProgram> ast = FrontEnd::parse(inputStream);

  GetVariableNode getVariableNode(token, position.line, start);
  ast->accept(&getVariableNode);

  std::stringstream ss;
  auto node = getVariableNode.getNode();
  if (node) {
    auto analysisResults = SemanticAnalysis::analyze(ast.get(), false);
    auto symbolTable = analysisResults->getSymbolTable();
    if (getVariableNode.isFunction()) {
      auto fnType =
          analysisResults->getTypeResults()->getInferredType(node.value());
      std::stringstream typeName;
      typeName << token << ": " << *fnType;
      return newHoverResponse(request.id, typeName.str());
    } else {
      auto varNode = symbolTable->getLocal(token, node.value());
      auto varType =
          analysisResults->getTypeResults()->getInferredType(varNode);

      std::stringstream typeName;
      typeName << token << ": " << *varType;
      return newHoverResponse(request.id, typeName.str());
    }
  } else {
    return newHoverResponse(request.id, "");
  }
  // auto analysisResults = SemanticAnalysis::analyze(ast.get(), false);
  // auto symbolTable = analysisResults->getSymbolTable();
  // // Figure out if it is a function
  // auto functions = symbolTable->getFunctions();
  // for (auto fn : functions) {
  //     if (fn->getName() == token) {
  //         if (fn->getLine() - 1 == position.line && fn->getColumn() == start)
  //         {
  //             auto fnType =
  //             analysisResults->getTypeResults()->getInferredType(fn);
  //             std::stringstream typeName;
  //             typeName << token << ": " << *fnType;
  //             return newHoverResponse(request.id, typeName.str());
  //         }
  //     }

  //     if (position.line < fn->getLine()) continue;

  //     auto locals = symbolTable->getLocals(fn);
  //     for (auto local : locals) {
  //         if (local->getName() == token) {
  //             if (local->getLine() - 1 == position.line && local->getColumn()
  //             == start) {
  //                 auto localType =
  //                 analysisResults->getTypeResults()->getInferredType(local);
  //                 std::stringstream typeName;
  //                 typeName << token << ": " << *localType;
  //                 return newHoverResponse(request.id, typeName.str());
  //             }
  //         }
  //     }
  // }
}
