#include "LSPState.h"
#include "ASTDeclNode.h"
#include "ASTProgram.h"
#include "CompletionRequest.h"
#include "FrontEnd.h"
#include "GetVariableNode.h"
#include "ParseError.h"
#include "RPC.h"
#include "SemanticAnalysis.h"
#include "SemanticError.h"
#include "TextDocument.h"
#include "loguru.hpp"
#include "messages.h"

#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <sstream>

auto runBackgroundSemanticAnalysis(
    std::string uri, std::shared_ptr<ASTProgram> ast, int version,
    std::mutex &lock,
    std::map<std::string, std::shared_ptr<SemanticAnalysis>> &analysisMap,
    std::map<std::string, std::optional<std::shared_ptr<SemanticAnalysis>>>
        &updatedAnalysisMap,
    std::map<std::string, int> &versionMap, std::mutex &writeLock) {
  std::function<void()> update = [uri, ast, version, &lock, &analysisMap,
                                  &updatedAnalysisMap, &versionMap,
                                  &writeLock]() {
    try {
      auto analysisResult = SemanticAnalysis::analyze(ast.get(), false);

      if (version == versionMap[uri]) {
        lock.lock();
        updatedAnalysisMap[uri] = analysisResult;
        analysisMap[uri] = analysisResult;
        lock.unlock();

        VersionedTextDocumentIdentfier documentIdentifier = {uri, version};
        auto diagnosticsNotification =
            newPublishDiagnosticsNotificationEmpty(documentIdentifier);
        auto message = EncodeMessage(diagnosticsNotification);

        writeLock.lock();
        std::cout << message;
        writeLock.unlock();
      }
    } catch (SemanticError e) {
      LOG_S(INFO) << e.what();
      if (version == versionMap[uri]) {

        VersionedTextDocumentIdentfier documentIdentifier = {uri, version};
        auto diagnosticsNotification = newPublishDiagnosticsNotificationError(
            documentIdentifier, e.what());
        auto message = EncodeMessage(diagnosticsNotification);

        writeLock.lock();
        std::cout << message;
        writeLock.unlock();
      }
    }
  };
  return std::async(std::launch::async, std::move(update));
}

std::optional<ParseError> LSPState::openDocument(std::string uri,
                                                 std::string text) {
  lock.lock();
  versions[uri] = 0;
  documents[uri] = text;
  std::stringstream stream(text);
  updatedAnalysisResults[uri] = std::nullopt;

  // Right now, the lsp will work off of the most recently properly parsed
  // document when providing information If the parsing fails, it will be marked
  // in the updatedAST map, some operations should work on the last correct AST
  // while others should fail to return information if the ast is not up to date
  try {
    asts[uri] = FrontEnd::parse(stream);
    updatedAst[uri] = true;
  } catch (ParseError e) {
    updatedAst[uri] = false;
    lock.unlock();
    return e;
  }
  lock.unlock();
  LOG_S(INFO) << "Running Semantic Analysis";
  runBackgroundSemanticAnalysis(uri, asts[uri], 0, lock, analysisResults,
                                updatedAnalysisResults, versions, writeLock);
  return std::nullopt;
}

std::optional<ParseError>
LSPState::updateDocument(std::string uri, std::string text, int version) {
  lock.lock();
  documents[uri] = text;
  versions[uri] = version;
  std::stringstream stream(text);
  updatedAnalysisResults[uri] = std::nullopt;
  try {
    asts[uri] = FrontEnd::parse(stream);
    updatedAst[uri] = true;
  } catch (ParseError e) {
    updatedAst[uri] = false;
    lock.unlock();
    return e;
  }
  lock.unlock();
  LOG_S(INFO) << "Running Semantic Analysis";
  runBackgroundSemanticAnalysis(uri, asts[uri], version, lock, analysisResults,
                                updatedAnalysisResults, versions, writeLock);
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
    if (updatedAnalysisResults[uri]) {
      analysisResult = updatedAnalysisResults[uri].value();
    } else {
      try {
        analysisResult = SemanticAnalysis::analyze(ast.get(), false);
        lock.lock();
        updatedAnalysisResults[uri] = analysisResult;
        lock.unlock();
      } catch (SemanticError e) {
        return newHoverResponse(request.id, "");
      }
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

CompletionResponse LSPState::autocomplete(CompletionRequest request) {
  auto uri = request.params.textDocument.uri;
  if (analysisResults.count(uri) == 0) {
    return newCompletionResponse(request.id, {});
  }

  std::vector<CompletionItem> items;

  std::shared_ptr<SemanticAnalysis> analysis = analysisResults[uri];
  std::shared_ptr<ASTProgram> ast = asts[uri];
  auto symbolTable = analysis->getSymbolTable();
  auto functions = symbolTable->getFunctions();
  auto typeInfo = analysis->getTypeResults();

  // Any function can be called at (almost) any time, so we supply all of them
  // May want to limit this to if in a function body
  for (const ASTDeclNode *fn : functions) {
    auto fnName = fn->getName();
    auto decl = ast->findFunctionByName(fnName);
    auto formals = decl->getFormals();

    std::ostringstream insertText;
    insertText << fnName << "(";

    if (formals.size() > 0) {
      int i = 0;
      for (; i < formals.size() - 1; i++) {
        auto formal = formals[i];
        insertText << "${" << i << ":" << formal->getName() << "}"
                   << ",";
      }
      insertText << "${" << i << ":" << formals[i]->getName() << "}";
    }
    insertText << ")";

    auto fnType = typeInfo->getInferredType((ASTDeclNode *)fn);
    std::ostringstream detail;
    detail << fnName << ": " << *fnType;
    items.push_back(CompletionItem{
        .label = fnName,
        .detail = detail.str(), // TOOD (Add more detail)
        .insertText = insertText.str(),
        .kind = CompletionItemKind::Function,
        .insertTextFormat = InsertTextFormat::Snippet,
    });
  }

  CompletionResponse response = newCompletionResponse(request.id, items);
  return response;
}
