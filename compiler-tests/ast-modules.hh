#ifndef AST_MODULES_HH_
# define AST_MODULES_HH_

#include "ast.hh"

class ASTProgram : public AST
{
public:
   ASTProgram(Token token, std::vector<AST*> children);

   virtual void accept(ASTVisitor& v) override;
};


#endif //!AST_MODULE_HH_
