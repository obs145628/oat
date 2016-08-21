#include "ast-modules.hh"
#include "ast-visitor.hh"
#include "ast-values.hh"
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


ASTComponent::ASTComponent(Token token, const std::vector<AST*>& children,
                           bool exported)
   : AST(token, children), _exported(exported)
{

}

bool ASTComponent::isExported() const
{
   return _exported;
}


ASTGlobalDef::ASTGlobalDef(Token t, ASTSymbol* symbol, AST* value, bool isConst,
                           bool exported)
   : ASTComponent(t, {symbol, value}, exported),
     _const(isConst)
{
   if(_const)
      assert(t.getType() == TokenType::kw_const);
   else
      assert(t.getType() == TokenType::kw_let);
}

ASTGlobalDef::ASTGlobalDef(Token t, ASTSymbol* symbol,
                           bool exported)
   : ASTComponent(t, {symbol}, exported), _const(false)
{
   assert(t.getType() == TokenType::kw_let);
}

void ASTGlobalDef::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbol* ASTGlobalDef::getSymbol() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

AST* ASTGlobalDef::getValue() const
{
   return _children[1];
}

bool ASTGlobalDef::hasValue() const
{
   return _children.size() > 1;
}

bool ASTGlobalDef::isConst() const
{
   return _const;
}
