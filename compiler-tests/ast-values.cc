#include "ast-values.hh"
#include "ast-visitor.hh"
#include <cassert>

ASTIntValue::ASTIntValue(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::integer));
}

long ASTIntValue::getValue() const
{
   return _token.getInt();
}

void ASTIntValue::accept(ASTVisitor& v)
{
   v.visit(this);
}


ASTVarValue::ASTVarValue(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::symbol));
}

std::string ASTVarValue::getVar() const
{
   return _token.getRepresentation();
}

void ASTVarValue::accept(ASTVisitor& v)
{
   v.visit(this);
}
