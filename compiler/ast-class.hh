#ifndef AST_CLASS_HH_
# define AST_CLASS_HH_

# include "ast.hh"
# include "ast-modules.hh"
# include <string>
# include "../vm/const.h"

class ASTClassField;
class ASTSymbol;
class ASTStatementsBlock;

class ASTClass : public ASTComponent
{
public:
   ASTClass(Token token, ASTSymbol* name, ASTSymbol* parent,
            const std::vector<ASTClassField*>& fields, bool exported);

   virtual void accept(ASTVisitor& v) override;

   bool hasParent() const;

   ASTSymbol* getName() const;
   ASTSymbol* getParent() const;
   size_t fieldsSize() const;
   ASTClassField* getField(size_t pos) const;

private:
   bool _parent;
};

class ASTClassField : public AST
{
public:
   ASTClassField(Token token, const std::vector<AST*>& children,
                 bool isStatic);

   bool isStatic() const;

private:
   bool _isStatic;
};

class ASTClassMethod : public ASTClassField
{
public:
   ASTClassMethod(Token token, ASTSymbol* name,
                  const std::vector<ASTSymbol*>& args,
                  ASTStatementsBlock* statement,
                  bool isStatic);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getName() const;
   ASTSymbol* getArg(size_t pos) const;
   std::vector<ASTSymbol*> getArgs() const;
   size_t argsSize() const;
   ASTStatementsBlock* getStatement();
};

class ASTClassVariable : public ASTClassField
{
public:
   ASTClassVariable(Token t, ASTSymbol* symbol, AST* value, bool isConst,
                    bool isStatic);
   ASTClassVariable(Token t, ASTSymbol* symbol,
                    bool isStatic);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getSymbol() const;
   AST* getValue() const;
   bool hasValue() const;
   bool isConst() const;

private:
   bool _const;
};


#endif //!AST_CLASS_HH_
