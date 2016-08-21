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

void ASTVisitorScope::visit(ASTStatementsBlock* e)
{
   assert(_parent);
   _scope = new RuntimeScope(e->getToken().getScanner(),
                             _parent->getFrame(), _parent);
}

void ASTVisitorScope::visit(ASTStatementFor* e)
{
   assert(_parent);
   _scope = new RuntimeScope(e->getToken().getScanner(),
                             _parent->getFrame(), _parent);
}

void ASTVisitorScope::visit(ASTModule* e)
{
   assert(!_parent);
   _scope = new RuntimeScope(e->getToken().getScanner(),
                             new StackFrame, nullptr);
}

void ASTVisitorScope::visit(ASTFunctionDef* e)
{
   assert(_parent);
   _scope = new RuntimeScope(e->getToken().getScanner(),
                             new StackFrame, _parent);
}

void ASTVisitorScope::visitDefault(AST*)
{
   _scope = _parent;
}
