#pragma once

#include "ASTDeclNode.h"
#include "ASTExpr.h"
#include "ASTFunction.h"
#include "ASTVariableExpr.h"
#include "ASTVisitor.h"
#include <optional>

class GetVariableNode : public ASTVisitor {
private:
  std::string name;
  int line;
  int column;
  ASTFunction *function;
  ASTFunction *currentFunction;
  bool isFn;
  bool valid;

public:
  GetVariableNode(std::string name, int line, int column)
      : name(name), line(line), column(column), valid(false),
        currentFunction(nullptr), isFn(false), function(nullptr) {}
  virtual bool visit(ASTFunction *element) override;
  virtual bool visit(ASTVariableExpr *element) override;
  virtual bool visit(ASTDeclNode *element) override;
  virtual bool visit(ASTFunAppExpr *element) override;
  std::optional<ASTDeclNode *> getNode();
  bool isFunction();
};
