#ifndef AST_MODULES_HH_
# define AST_MODULES_HH_

# include "ast.hh"

class ASTModule : public AST
{

public:
   ASTModule(Token token, const std::vector<AST*>& children);

   virtual void accept(ASTVisitor& v) override;
};

#endif //!AST_MODULES_HH_
