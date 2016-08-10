#ifndef AST_FUNCTIONS_HH_
# define AST_FUNCTIONS_HH_

#include "ast.hh"

class ASTFunctionDef : public AST
{
public:
   ASTFunctionDef(Token token, std::vector<AST*> children);

   virtual void accept(ASTVisitor& v) override;

   size_t getArgsSize() const;
   std::string getFunctionName() const;
   std::string getArgName(size_t pos) const;
   AST* getStatement() const;

};

class ASTFunctionCall : public AST
{
public:
   ASTFunctionCall(Token token, std::vector<AST*> children);

   virtual void accept(ASTVisitor& v) override;

   size_t getArgsSize() const;
   std::string getFunctionName() const;
   AST* getArgValue(size_t pos) const;

};

#endif //!AST_FUNCTIONS_HH_
