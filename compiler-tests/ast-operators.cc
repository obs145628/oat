#include "ast-operators.hh"
#include "ast-visitor.hh"

ASTOp1Plus::ASTOp1Plus(Token token, AST* ast)
   : AST(token, {ast})
{

}

void ASTOp1Plus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp1Minus::ASTOp1Minus(Token token, AST* ast)
   : AST(token, {ast})
{

}

void ASTOp1Minus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Plus::ASTOp2Plus(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTOp2Plus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Minus::ASTOp2Minus(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTOp2Minus::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Mul::ASTOp2Mul(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTOp2Mul::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Div::ASTOp2Div(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTOp2Div::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTOp2Mod::ASTOp2Mod(Token token, AST* left, AST* right)
   : AST(token, {left, right})
{

}

void ASTOp2Mod::accept(ASTVisitor& v)
{
   v.visit(this);
}
