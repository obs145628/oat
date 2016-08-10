#ifndef AST_VALUES_HH_
#define AST_VALUES_HH_

#include "ast.hh"

class ASTIntValue : public AST
{
public:
   ASTIntValue(Token token);

   long getValue() const;

   virtual void accept(ASTVisitor& v) override;
};

class ASTVarValue : public AST
{
public:
   ASTVarValue(Token token);

   std::string getVar() const;

   virtual void accept(ASTVisitor& v) override;
};

#endif //!AST_VALUES_HH_
