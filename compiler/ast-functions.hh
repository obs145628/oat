#ifndef AST_FUNCTIONS_HH_
# define AST_FUNCTIONS_HH_

# include "ast.hh"

class ASTSymbol;
class ASTStatementsBlock;

class ASTFunctionDef : public AST
{
public:
   ASTFunctionDef(Token token, ASTSymbol* name,
                  const std::vector<ASTSymbol*>& args,
                  ASTStatementsBlock* statement);

   virtual void accept(ASTVisitor& v) override;

   ASTSymbol* getName() const;
   ASTSymbol* getArg(size_t pos) const;
   std::vector<ASTSymbol*> getArgs() const;
   size_t argsSize() const;
   ASTStatementsBlock* getStatement();

};


#endif //!AST_FUNCTIONS_HH_
