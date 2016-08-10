#include "ast-statements.hh"
#include "ast-visitor.hh"

ASTStatementsList::ASTStatementsList(Token token, std::vector<AST*> list)
   : AST(token, list)
{

}

void ASTStatementsList::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTStatementEmpty::ASTStatementEmpty(Token token)
   : AST(token)
{

}

void ASTStatementEmpty::accept(ASTVisitor& v)
{
   v.visit(this);
}


ASTStatementLet::ASTStatementLet(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTStatementLet::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTStatementPrint::ASTStatementPrint(Token token, AST* ast)
   : AST(token, {ast})
{

}

void ASTStatementPrint::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTStatementReturn::ASTStatementReturn(Token token, AST* ast)
   : AST(token, {ast})
{

}

void ASTStatementReturn::accept(ASTVisitor& v)
{
   v.visit(this);
}
