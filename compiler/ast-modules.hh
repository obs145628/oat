#ifndef AST_MODULES_HH_
# define AST_MODULES_HH_

# include "ast.hh"

class ASTSymbol;

class ASTModule : public AST
{

public:
   ASTModule(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;
};

class ASTComponent : public AST
{

public:
   ASTComponent(Token token, const std::vector<AST*>& children,
                bool exported, int module);

   bool isExported() const;
   int getModule() const;

private:
   bool _exported;
   int _module;
};


class ASTGlobalDef : public ASTComponent
{
public:
   ASTGlobalDef(Token t, ASTSymbol* symbol, AST* value, bool isConst,
                bool exported, int module);
   ASTGlobalDef(Token t, ASTSymbol* symbol,
                bool exported, int module);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getSymbol() const;
   AST* getValue() const;
   bool hasValue() const;
   bool isConst() const;

private:
   bool _const;
};

#endif //!AST_MODULES_HH_
