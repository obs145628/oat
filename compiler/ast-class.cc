#include "ast-class.hh"
#include "ast-visitor.hh"
#include "ast-values.hh"
#include <cassert>

ASTClass::ASTClass(Token token, ASTSymbol* name, ASTSymbol* parent,
                   const std::vector<ASTClassField*>& fields, bool exported)
   : ASTComponent(token, {name}, exported), _parent(!!parent)
{
   assert(token.isOfType(TokenType::kw_class));
   if(_parent)
      _children.push_back(parent);
   for(ASTClassField* f : fields)
      _children.push_back(f);
}

void ASTClass::accept(ASTVisitor& v)
{
   v.visit(this);
}

bool ASTClass::hasParent() const
{
   return _parent;
}

ASTSymbol* ASTClass::getName() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

ASTSymbol* ASTClass::getParent() const
{
   assert(_parent);
   return dynamic_cast<ASTSymbol*> (_children[1]);
}

size_t ASTClass::fieldsSize() const
{
   size_t dec = _parent ? 2 : 1;
   return _children.size() - dec;
}

ASTClassField* ASTClass::getField(size_t pos) const
{
   if(_parent)
      ++pos;
   assert(pos + 1 < _children.size());
   return dynamic_cast<ASTClassField*> (_children[pos+1]);
}

ASTClassField::ASTClassField(Token token, const std::vector<AST*>& children,
                             bool isStatic)
   : AST(token, children), _isStatic(isStatic)
{

}

bool ASTClassField::isStatic() const
{
   return _isStatic;
}


ASTClassMethod::ASTClassMethod(Token token, ASTSymbol* name,
                               const std::vector<ASTSymbol*>& args,
                               ASTStatementsBlock* statement,
                               bool isStatic)
   : ASTClassField(token, {}, isStatic)
{
   assert(token.getType() == TokenType::symbol);

   _children.push_back(name);
   for(ASTSymbol* arg: args)
      _children.push_back(arg);
   _children.push_back(statement);
}

void ASTClassMethod::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbol* ASTClassMethod::getName() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

ASTSymbol* ASTClassMethod::getArg(size_t pos) const
{
   assert(pos < _children.size() - 2);
   return dynamic_cast<ASTSymbol*> (_children[pos + 1]);
}

std::vector<ASTSymbol*> ASTClassMethod::getArgs() const
{
   std::vector<ASTSymbol*> args;
   for(size_t i = 1; i < _children.size() - 1; ++i)
      args.push_back(dynamic_cast<ASTSymbol*> (_children[i]));

   return args;
}

size_t ASTClassMethod::argsSize() const
{
   return _children.size() - 2;
}

ASTStatementsBlock* ASTClassMethod::getStatement()
{
   return dynamic_cast<ASTStatementsBlock*> (_children[_children.size() - 1]);
}


ASTClassVariable::ASTClassVariable(Token t, ASTSymbol* symbol, AST* value,
                                   bool isConst, bool isStatic)
   : ASTClassField(t, {symbol, value}, isStatic),
     _const(isConst)
{
   if(_const)
      assert(t.getType() == TokenType::kw_const);
   else
      assert(t.getType() == TokenType::kw_let);
}

ASTClassVariable::ASTClassVariable(Token t, ASTSymbol* symbol,
                                   bool isStatic)
   : ASTClassField(t, {symbol}, isStatic), _const(false)
{
   assert(t.getType() == TokenType::kw_let);
}

void ASTClassVariable::accept(ASTVisitor& v)
{
   v.visit(this);
}

ASTSymbol* ASTClassVariable::getSymbol() const
{
   return dynamic_cast<ASTSymbol*> (_children[0]);
}

AST* ASTClassVariable::getValue() const
{
   return _children[1];
}

bool ASTClassVariable::hasValue() const
{
   return _children.size() > 1;
}

bool ASTClassVariable::isConst() const
{
   return _const;
}
