#include "ast-modules.hh"
#include "ast-visitor.hh"
#include <cassert>

ASTModule::ASTModule(Token token, const std::vector<AST*>& children)
   : AST(token, children)
{
   assert(token.getType() == TokenType::eof);
}

void ASTModule::accept(ASTVisitor& v)
{
   v.visit(this);
}
