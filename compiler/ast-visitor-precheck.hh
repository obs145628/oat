#ifndef AST_VISTOR_PRECHECK_HH_
# define AST_VISTOR_PRECHECK_HH_

#include "ast-visitor.hh"

class ASTState;

class ASTVisitorPrecheck : public ASTVisitor
{

public:
   ASTVisitorPrecheck(ASTState* state);

   virtual void visit(ASTFunctionDef* e) override;

private:
   ASTState* _state;

   void visitChildren();

   virtual void visitDefault(AST* e) override;


};

# endif //!AST_VISTOR_PRECHECK_HH_
