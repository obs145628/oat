#include "ast-values.hh"
#include "ast-visitor.hh"
#include <cassert>

ASTInt::ASTInt(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::integer));
}

t_vm_int ASTInt::getValue() const
{
   return static_cast<t_vm_int> (_token.getInt());
}

void ASTInt::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTDouble::ASTDouble(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::real));
}

t_vm_double ASTDouble::getValue() const
{
   return static_cast<t_vm_double> (_token.getReal());
}

void ASTDouble::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTChar::ASTChar(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::litteral1));
}

t_vm_char ASTChar::getValue() const
{
   return static_cast<t_vm_char> (_token.getLitteral1());
}

void ASTChar::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTString::ASTString(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::litteral2));
}

std::string ASTString::getValue() const
{
   return _token.getLitteral2();
}

void ASTString::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTTrue::ASTTrue(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::kw_true));
}

void ASTTrue::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTFalse::ASTFalse(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::kw_false));
}

void ASTFalse::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTNull::ASTNull(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::kw_null));
}

void ASTNull::accept(ASTVisitor& v)
{
   v.visit(this);
}


ASTSymbol::ASTSymbol(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::symbol));
}

std::string ASTSymbol::getName() const
{
   return _token.getRepresentation();
}

void ASTSymbol::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbolValue::ASTSymbolValue(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::symbol));
}

std::string ASTSymbolValue::getName() const
{
   return _token.getRepresentation();
}

void ASTSymbolValue::accept(ASTVisitor& v)
{
   v.visit(this);
}


ASTArray::ASTArray(Token token, const std::vector<AST*>& children)
   : AST(token, children)
{
   assert(token.getType() == TokenType::lbracket);
}

void ASTArray::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSet::ASTSet(Token token, const std::vector<AST*>& children)
   : AST(token, children)
{
   assert(token.getType() == TokenType::lcurlybracket);
}

void ASTSet::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTMap::ASTMap(Token token, const std::vector<AST*>& children)
   : AST(token, children)
{
   assert(token.getType() == TokenType::lcurlybracket);
   assert(children.size() % 2 == 0);
}

void ASTMap::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTThis::ASTThis(Token token)
   : AST(token)
{
   assert(token.isOfType(TokenType::kw_this));
}

void ASTThis::accept(ASTVisitor& v)
{
   v.visit(this);
}
