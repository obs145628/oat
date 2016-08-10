#include "ast-modules.hh"
#include "ast-visitor.hh"

ASTProgram::ASTProgram(Token token, std::vector<AST*> children)
   : AST(token, children)
{

}

void ASTProgram::accept(ASTVisitor& v)
{
   v.visit(this);
}
