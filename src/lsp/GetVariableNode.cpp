#include "GetVariableNode.h"
#include "ASTDeclNode.h"
#include "ASTFunction.h"
#include "ASTVariableExpr.h"
#include "loguru.hpp"

#include <optional>

bool GetVariableNode::visit(ASTFunction *element) {
  if (valid)
    return false;
  currentFunction = element;
  if (element->getName() == name && element->getLine() - 1 == line &&
      element->getColumn() == column) {
    isFn = true;
    valid = true;
    function = currentFunction;
    return false;
  }

  return true;
}

bool GetVariableNode::visit(ASTVariableExpr *element) {
  if (valid)
    return false;
  if (element->getName() == name && element->getLine() - 1 == line &&
      element->getColumn() == column) {
    isFn = false;
    valid = true;
    function = currentFunction;
    return false;
  }
  return true;
}

bool GetVariableNode::visit(ASTDeclNode *element) {
  if (valid)
    return false;
  if (element->getName() == name && element->getLine() - 1 == line &&
      element->getColumn() == column) {
    isFn = false;
    valid = true;
    function = currentFunction;
    return false;
  }
  return true;
}

bool GetVariableNode::visit(ASTFunAppExpr *element) {
  if (valid)
    return false;
  auto fn = element->getFunction();
  if (fn->getLine() - 1 == line && fn->getColumn() == column) {
    isFn = true;
    valid = true;
    function = currentFunction;
    return false;
  }
  return true;
}

std::optional<ASTDeclNode *> GetVariableNode::getNode() {
  if (valid) {
    return function->getDecl();
  }
  return std::nullopt;
}

bool GetVariableNode::isFunction() { return isFn; }
