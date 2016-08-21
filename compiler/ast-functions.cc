#include "ast-functions.hh"
#include "ast-visitor.hh"
#include "ast-values.hh"
#include "ast-statements.hh"
#include <cassert>

ASTFunctionDef::ASTFunctionDef(Token token, ASTSymbol* name,
                               const std::vector<ASTSymbol*>& args,
                               ASTStatementsBlock* statement,
                               bool exported)
   : ASTComponent(token, {}, exported)
{
   assert(token.getType() == TokenType::symbol);

   _children.push_back(name);
   for(ASTSymbol* arg: args)
      _children.push_back(arg);
   _children.push_back(statement);
}

void ASTFunctionDef::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbol* ASTFunctionDef::getName() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

ASTSymbol* ASTFunctionDef:: getArg(size_t pos) const
{
   assert(pos < _children.size() - 2);
   return dynamic_cast<ASTSymbol*> (_children[pos + 1]);
}

std::vector<ASTSymbol*> ASTFunctionDef::getArgs() const
{
   std::vector<ASTSymbol*> args;
   for(size_t i = 1; i < _children.size() - 1; ++i)
      args.push_back(dynamic_cast<ASTSymbol*> (_children[i]));

   return args;
}

size_t ASTFunctionDef::argsSize() const
{
   return _children.size() - 2;
}

ASTStatementsBlock* ASTFunctionDef::getStatement()
{
   return dynamic_cast<ASTStatementsBlock*> (_children[_children.size() - 1]);
}
