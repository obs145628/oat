#include "ast-statements.hh"
#include "ast-visitor.hh"
#include "ast-values.hh"
#include <cassert>

ASTStatementsBlock::ASTStatementsBlock(Token t,
                                       const std::vector<AST*> children)
   : AST(t, children)
{

}

void ASTStatementsBlock::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTStatementEmpty::ASTStatementEmpty(Token t)
   : AST(t)
{
   assert(t.getType() == TokenType::semicollon);
}

void ASTStatementEmpty::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTStatementDefine::ASTStatementDefine(Token t, ASTSymbol* symbol, AST* value)
   : AST(t, {symbol, value})
{
   assert(t.getType() == TokenType::si_define);
}

void ASTStatementDefine::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbol* ASTStatementDefine::getSymbol() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

AST* ASTStatementDefine::getValue() const
{
   return _children[1];
}

ASTStatementReturn::ASTStatementReturn(Token t, AST* value)
   : AST(t, {value})
{
   assert(t.getType() == TokenType::si_return);
}

void ASTStatementReturn::accept(ASTVisitor& v)
{
   v.visit(this);
}

AST* ASTStatementReturn::getValue() const
{
   return _children[0];
}



ASTStatementIf::ASTStatementIf(Token t, AST* condition,
                               ASTStatementsBlock* ifStatement)
   : AST(t, {condition, ifStatement})
{
   assert(t.getType() == TokenType::kw_if);
}

ASTStatementIf::ASTStatementIf(Token t, AST* condition,
                               ASTStatementsBlock* ifStatement,
                               ASTStatementsBlock* elseStatement)
   : AST(t, {condition, ifStatement, elseStatement})
{
   assert(t.getType() == TokenType::kw_if);
}

void ASTStatementIf::accept(ASTVisitor& v)
{
   v.visit(this);
}

AST* ASTStatementIf::getCondition() const
{
   return _children[0];
}

ASTStatementsBlock* ASTStatementIf::getIfStatement() const
{
   return dynamic_cast<ASTStatementsBlock*>(_children[1]);
}

ASTStatementsBlock* ASTStatementIf::getElseStatement() const
{
   assert(hasElseStatement());
   return dynamic_cast<ASTStatementsBlock*>(_children[2]);
}

bool ASTStatementIf::hasElseStatement() const
{
   return _children.size() == 3;
}

ASTStatementWhile::ASTStatementWhile(Token t, AST* condition,
                                     ASTStatementsBlock* whileStatement)
   : AST(t, {condition, whileStatement})
{
   assert(t.getType() == TokenType::kw_while);
}

void ASTStatementWhile::accept(ASTVisitor& v)
{
   v.visit(this);
}

AST* ASTStatementWhile::getCondition() const
{
   return _children[0];
}

ASTStatementsBlock* ASTStatementWhile::getWhileStatement() const
{
   return dynamic_cast<ASTStatementsBlock*> (_children[1]);
}
