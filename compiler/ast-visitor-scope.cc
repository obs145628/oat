#include "ast-visitor-scope.hh"
#include "runtime-scope.hh"
#include <cassert>

ASTVisitorScope::ASTVisitorScope(AST* ast, RuntimeScope* parent)
   : _parent(parent)
{
   ast->accept(*this);
}

RuntimeScope* ASTVisitorScope::getScope(AST* ast, RuntimeScope* parent)
{
   return ASTVisitorScope(ast, parent)._scope;
}

void ASTVisitorScope::visit(ASTStatementsBlock*)
{
   assert(_parent);
   _scope = new RuntimeScope(_parent->getFrame(), _parent);
}

void ASTVisitorScope::visit(ASTStatementFor*)
{
   assert(_parent);
   _scope = new RuntimeScope(_parent->getFrame(), _parent);
}

void ASTVisitorScope::visit(ASTModule*)
{
   assert(!_parent);
   _scope = new RuntimeScope(new StackFrame, nullptr);
}

void ASTVisitorScope::visit(ASTFunctionDef*)
{
   assert(_parent);
   _scope = new RuntimeScope(new StackFrame, _parent);
}

void ASTVisitorScope::visitDefault(AST*)
{
   _scope = _parent;
}
