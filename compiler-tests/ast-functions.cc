#include "ast-functions.hh"
#include "ast-visitor.hh"
#include <stdexcept>
#include <cassert>

ASTFunctionDef::ASTFunctionDef(Token token, std::vector<AST*> children)
   : AST(token, children)
{

}

void ASTFunctionDef::accept(ASTVisitor& v)
{
   v.visit(this);
}

size_t ASTFunctionDef::getArgsSize() const
{
   return _children.size() - 2;
}

std::string ASTFunctionDef::getFunctionName() const
{
   return _children[0]->getToken().getRepresentation();
}

std::string ASTFunctionDef::getArgName(size_t pos) const
{
   assert(pos < _children.size() - 2);
   return _children[pos + 1]->getToken().getRepresentation();
}

AST* ASTFunctionDef::getStatement() const
{
   return _children[_children.size() - 1];
}


ASTFunctionCall::ASTFunctionCall(Token token, std::vector<AST*> children)
   : AST(token, children)
{

}

void ASTFunctionCall::accept(ASTVisitor& v)
{
   v.visit(this);
}

size_t ASTFunctionCall::getArgsSize() const
{
   return _children.size() - 1;
}

std::string ASTFunctionCall::getFunctionName() const
{
   return _children[0]->getToken().getRepresentation();
}

AST* ASTFunctionCall::getArgValue(size_t pos) const
{
   assert(pos < _children.size() - 1);
   return _children[pos + 1];
}
